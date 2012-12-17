/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1985 Thomas L. Quarles
Modified: 2000 AlansFixes
**********/

#include "ngspice/ngspice.h"
#include "ngspice/cktdefs.h"
#include "vsrcdefs.h"
#include "ngspice/trandefs.h"
#include "ngspice/sperror.h"
#include "ngspice/suffix.h"
#include "ngspice/1-f-code.h"

#ifdef XSPICE_EXP
/* gtri - begin - wbk - modify for supply ramping option */
#include "ngspice/cmproto.h"
/* gtri - end   - wbk - modify for supply ramping option */
#endif


static double
pwl_state_get(struct pwl_state *this, double time, double td, double tp, int XrBreakpt)
{
    /* fixme, enter mehrmals im sägezahn ...
     *   1) optimier dafür
     *   2) stelle sicher, dass das jeweils höchste timestamp zum zug kommt
     *       (allow step antwort *)
     * invariant bei repeat:
     *   this->position < indexof(last-point) (fixme really ? wenn es nur 2 points sind zB )
     */

    /* fixme, einen warp einbauen wenns nicht passt ? */

    for (;;) {

        /* carefully exactly duplicate the math in vsrcacct .. */

        volatile double t1 = this->arr[this->position + 0] + td + (this->rpt_cnt * tp);
        volatile double t2 = this->arr[this->position + 2] + td + (this->rpt_cnt * tp);

        double v1, v2;

        if (time >= t2) {
            if (this->position+4 < this->len) {
                this->position += 2;
                continue;
            }

            if (tp == 0.0)
                return this->arr[this->len-1];

            /* actually a += 4 step, but the intermediate
             *   suffices t1 == t2
             */

            this->position = XrBreakpt;
            this->rpt_cnt++;
            continue;
        }

        if (time < t1) {
            if (this->position > XrBreakpt) { /*fixme must be 0 !! vfor non repeat*/
                this->position -= 2;
                continue;
            }

            if (tp == 0.0 || this->rpt_cnt == 0)
                return this->arr[1];

            /* actually a -= 4 step, but ... */

            this->position = this->len - 4;
            this->rpt_cnt--;
            continue;
        }

        /* invariant:  t1 <= time < t2  ==> t1 != t2 */

        v1 = this->arr[this->position + 1];
        v2 = this->arr[this->position + 3];

        return v1 + ((v2-v1)/(t2-t1)) * (time - t1);
    }
}


static void
pwl_state_init(struct pwl_state *this, VSRCinstance *here)
{

    this->len = here->VSRCfunctionOrder;
    this->arr = here->VSRCcoeffs;
    this->position = 0;
    this->rpt_cnt = 0;
}


/*-----------------------------------------------------------------------------*/


int
VSRCload(GENmodel *inModel, CKTcircuit *ckt)
        /* actually load the current value into the
         * sparse matrix previously provided
         */
{
    VSRCmodel *model = (VSRCmodel *) inModel;
    VSRCinstance *here;
    double time;
    double value = 0.0;

    /*  loop through all the source models */
    for( ; model != NULL; model = model->VSRCnextModel ) {

        /* loop through all the instances of the model */
        for (here = model->VSRCinstances; here != NULL ;
                here=here->VSRCnextInstance) {

            *(here->VSRCposIbrptr) += 1.0 ;
            *(here->VSRCnegIbrptr) -= 1.0 ;
            *(here->VSRCibrPosptr) += 1.0 ;
            *(here->VSRCibrNegptr) -= 1.0 ;
            if( (ckt->CKTmode & (MODEDCOP | MODEDCTRANCURVE)) &&
                    here->VSRCdcGiven ) {
                /* load using DC value */
#ifdef XSPICE_EXP
/* gtri - begin - wbk - modify to process srcFact, etc. for all sources */
                value = here->VSRCdcValue;
#else
                value = here->VSRCdcValue * ckt->CKTsrcFact;
#endif
            } else {
                if(ckt->CKTmode & (MODEDC)) {
                    time = 0;
                } else {
                    time = ckt->CKTtime;
                }
                /* use the transient functions */
                switch(here->VSRCfunctionType) {

                    default:
                        value = here->VSRCdcValue;
                        break;

                    case PULSE: {
                        double V1, V2, TD, TR, TF, PW, PER;
                        double basetime = 0;
#ifdef XSPICE
                        double PHASE;
                        double phase;
                        double deltat;
#endif
                        V1 = here->VSRCcoeffs[0];
                        V2 = here->VSRCcoeffs[1];
                        TD = here->VSRCfunctionOrder > 2
                           ? here->VSRCcoeffs[2] : 0.0;
                        TR = here->VSRCfunctionOrder > 3
                           && here->VSRCcoeffs[3] != 0.0
                           ? here->VSRCcoeffs[3] : ckt->CKTstep;
                        TF = here->VSRCfunctionOrder > 4
                           && here->VSRCcoeffs[4] != 0.0
                           ? here->VSRCcoeffs[4] : ckt->CKTstep;
                        PW = here->VSRCfunctionOrder > 5
                           && here->VSRCcoeffs[5] != 0.0
                           ? here->VSRCcoeffs[5] : ckt->CKTfinalTime;
                        PER = here->VSRCfunctionOrder > 6
                           && here->VSRCcoeffs[6] != 0.0
                           ? here->VSRCcoeffs[6] : ckt->CKTfinalTime;

                        /* shift time by delay time TD */
                        time -=  TD;

#ifdef XSPICE
/* gtri - begin - wbk - add PHASE parameter */
                        PHASE = here->VSRCfunctionOrder > 7
                           ? here->VSRCcoeffs[7] : 0.0;

                        /* normalize phase to cycles */
                        phase = PHASE / 360.0;
                        phase = fmod(phase, 1.0);
                        deltat =  phase * PER;
                        while (deltat > 0)
                            deltat -= PER;
                        /* shift time by pase (neg. for pos. phase value) */
                        time += deltat;
/* gtri - end - wbk - add PHASE parameter */
#endif
                        if(time > PER) {
                            /* repeating signal - figure out where we are */
                            /* in period */
                            basetime = PER * floor(time/PER);
                            time -= basetime;
                        }
                        if (time <= 0 || time >= TR + PW + TF) {
                            value = V1;
                        } else  if (time >= TR && time <= TR + PW) {
                            value = V2;
                        } else if (time > 0 && time < TR) {
                            value = V1 + (V2 - V1) * (time) / TR;
                        } else { /* time > TR + PW && < TR + PW + TF */
                            value = V2 + (V1 - V2) * (time - (TR + PW)) / TF;
                        }
                    }
                    break;

                    case SINE: {

                        double VO, VA, FREQ, TD, THETA;
/* gtri - begin - wbk - add PHASE parameter */
#ifdef XSPICE
                        double PHASE;
                        double phase;

                        PHASE = here->VSRCfunctionOrder > 5
                           ? here->VSRCcoeffs[5] : 0.0;

                        /* compute phase in radians */
                        phase = PHASE * M_PI / 180.0;
#endif
                        VO = here->VSRCcoeffs[0];
                        VA = here->VSRCcoeffs[1];
                        FREQ =  here->VSRCfunctionOrder > 2
                           && here->VSRCcoeffs[2] != 0.0
                           ? here->VSRCcoeffs[2] : (1/ckt->CKTfinalTime);
                        TD = here->VSRCfunctionOrder > 3
                           ? here->VSRCcoeffs[3] : 0.0;
                        THETA = here->VSRCfunctionOrder > 4
                           ? here->VSRCcoeffs[4] : 0.0;

                        time -= TD;
                        if (time <= 0) {
#ifdef XSPICE
                            value = VO + VA * sin(phase);
                        } else {
                            value = VO + VA * sin(FREQ*time * 2.0 * M_PI + phase) *
                                exp(-time*THETA);
#else
                            value = VO;
                        } else {
                            value = VO + VA * sin(FREQ * time * 2.0 * M_PI) *
                                exp(-time*THETA);
#endif
/* gtri - end - wbk - add PHASE parameter */
                        }
                    }
                    break;

                    case EXP: {
                        double V1, V2, TD1, TD2, TAU1, TAU2;

                        V1  = here->VSRCcoeffs[0];
                        V2  = here->VSRCcoeffs[1];
                        TD1 = here->VSRCfunctionOrder > 2
                           && here->VSRCcoeffs[2] != 0.0
                           ? here->VSRCcoeffs[2] : ckt->CKTstep;
                        TAU1 = here->VSRCfunctionOrder > 3
                           && here->VSRCcoeffs[3] != 0.0
                           ? here->VSRCcoeffs[3] : ckt->CKTstep;
                        TD2  = here->VSRCfunctionOrder > 4
                           && here->VSRCcoeffs[4] != 0.0
                           ? here->VSRCcoeffs[4] : TD1 + ckt->CKTstep;
                        TAU2 = here->VSRCfunctionOrder > 5
                           && here->VSRCcoeffs[5]
                           ? here->VSRCcoeffs[5] : ckt->CKTstep;

                        if(time <= TD1)  {
                            value = V1;
                        } else if (time <= TD2) {
                            value = V1 + (V2-V1)*(1-exp(-(time-TD1)/TAU1));
                        } else {
                            value = V1 + (V2-V1)*(1-exp(-(time-TD1)/TAU1)) +
                                         (V1-V2)*(1-exp(-(time-TD2)/TAU2)) ;
                        }
                    }
                    break;

                    case SFFM: {

                        double VO, VA, FC, MDI, FS;
/* gtri - begin - wbk - add PHASE parameters */
#ifdef XSPICE
                        double PHASEC, PHASES;
                        double phasec;
                        double phases;

                        PHASEC = here->VSRCfunctionOrder > 5
                            ? here->VSRCcoeffs[5] : 0.0;
                        PHASES = here->VSRCfunctionOrder > 6
                            ? here->VSRCcoeffs[6] : 0.0;

                        /* compute phases in radians */
                        phasec = PHASEC * M_PI / 180.0;
                        phases = PHASES * M_PI / 180.0;
#endif

                        VO = here->VSRCcoeffs[0];
                        VA = here->VSRCcoeffs[1];
                        FC = here->VSRCfunctionOrder > 2
                           && here->VSRCcoeffs[2]
                           ? here->VSRCcoeffs[2] : (1/ckt->CKTfinalTime);
                        MDI = here->VSRCfunctionOrder > 3
                           ? here->VSRCcoeffs[3] : 0.0;
                        FS  = here->VSRCfunctionOrder > 4
                           && here->VSRCcoeffs[4]
                           ? here->VSRCcoeffs[4] : (1/ckt->CKTfinalTime);

#ifdef XSPICE
                        /* compute waveform value */
                        value = VO + VA *
                            sin((2.0 * M_PI * FC * time + phasec) +
                            MDI * sin(2.0 * M_PI * FS * time + phases));
#else
                        value = VO + VA *
                            sin((2.0 * M_PI * FC * time) +
                            MDI * sin(2.0 * M_PI * FS * time));
#endif
/* gtri - end - wbk - add PHASE parameters */

                    }
                    break;

                    case AM: {

                        double VA, FC, MF, VO, TD;
/* gtri - begin - wbk - add PHASE parameters */
#ifdef XSPICE
                        double PHASEC, PHASES;
                        double phasec;
                        double phases;

                        PHASEC = here->VSRCfunctionOrder > 5
                            ? here->VSRCcoeffs[5] : 0.0;
                        PHASES = here->VSRCfunctionOrder > 6
                            ? here->VSRCcoeffs[6] : 0.0;

                        /* compute phases in radians */
                        phasec = PHASEC * M_PI / 180.0;
                        phases = PHASES * M_PI / 180.0;
#endif

                        VA = here->VSRCcoeffs[0];
                        VO = here->VSRCcoeffs[1];
                        MF = here->VSRCfunctionOrder > 2
                           && here->VSRCcoeffs[2]
                           ? here->VSRCcoeffs[2] : (1/ckt->CKTfinalTime);
                        FC = here->VSRCfunctionOrder > 3
                           ? here->VSRCcoeffs[3] : 0.0;
                        TD  = here->VSRCfunctionOrder > 4
                           && here->VSRCcoeffs[4]
                           ? here->VSRCcoeffs[4] : 0.0;

                        time -= TD;
                        if (time <= 0) {
                            value = 0;
                        } else {
#ifdef XSPICE
                            /* compute waveform value */
                            value = VA * (VO + sin(2.0 * M_PI * MF * time + phases )) *
                                sin(2.0 * M_PI * FC * time + phases);

#else
                            value = VA * (VO + sin(2.0 * M_PI * MF * time)) *
                                sin(2.0 * M_PI * FC * time);
#endif
                        }

/* gtri - end - wbk - add PHASE parameters */
                    }
                    break;

                    case PWL: {

                        struct pwl_state *state;

                        double td = here->VSRCrdelay;
                        double tp = here->VSRCrperiod;

                        if (!here->VSRC_state) {
                            here->VSRC_state = TMALLOC(struct pwl_state, 1);
                            pwl_state_init((struct pwl_state *) here->VSRC_state, here);
                        }

                        state = (struct pwl_state *) here -> VSRC_state;

                        if (here->VSRCrGiven) {
                            value = pwl_state_get(state, time, td, tp, here->VSRCrBreakpt);
                        } else {
                            value = pwl_state_get(state, time, td, 0.0, 0);
                        }
                    }
                    break;

/**** tansient noise routines:
VNoi2 2 0  DC 0 TRNOISE(10n 0.5n 0 0n) : generate gaussian distributed noise
                        rms value, time step, 0 0
VNoi1 1 0  DC 0 TRNOISE(0n 0.5n 1 10n) : generate 1/f noise
                        0,  time step, exponent < 2, rms value

VNoi3 3 0  DC 0 TRNOISE(0 0 0 0 15m 22u 50u) : generate RTS noise
                        0 0 0 0, amplitude, capture time, emission time
*/
                    case TRNOISE: {

                        struct trnoise_state *state = here -> VSRCtrnoise_state;

                        double TS = state -> TS;
                        double RTSAM = state->RTSAM;

                        /* no noise */
                        if(TS == 0.0) {
                            value = 0.0;
                        } else {
                            /* 1/f and white noise */
                            size_t n1 = (size_t) floor(time / TS);

                            double V1 = trnoise_state_get(state, ckt, n1);
                            double V2 = trnoise_state_get(state, ckt, n1+1);

                            value = V1 + (V2 - V1) * (time / TS - (double)n1);
                        }

                        /* RTS noise */
                        if (RTSAM > 0) {
                            double RTScapTime = state->RTScapTime;
                            if (time >= RTScapTime)
                                value += RTSAM;
                        }

                        /* DC value */
                        if(here -> VSRCdcGiven)
                            value += here->VSRCdcValue;
                    }
                    break;

                    case TRRANDOM: {
                        struct trrandom_state *state = here -> VSRCtrrandom_state;
                        value = state -> value;
                        /* DC value */
                        if(here -> VSRCdcGiven)
                            value += here->VSRCdcValue;
                    }
                    break;

                } // switch
            } // else (line 48)

/* gtri - begin - wbk - modify for supply ramping option */
#ifdef XSPICE_EXP
            value *= ckt->CKTsrcFact;
            value *= cm_analog_ramp_factor();
#else
            if (ckt->CKTmode & MODETRANOP)
                value *= ckt->CKTsrcFact;
#endif
/* gtri - end - wbk - modify to process srcFact, etc. for all sources */

            /* load the new voltage value into the matrix */
            *(ckt->CKTrhs + (here->VSRCbranch)) += value;

        } // for loop instances
    } // for loop models

    return(OK);
}
