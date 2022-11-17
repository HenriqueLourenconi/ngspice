OSDI Capacitor Test
.options abstol=1e-15


* one voltage source for sweeping, one for sensing:
VD Dx 0 DC 0 AC 1 SIN (0.5 0.2 1M)
Vsense Dx D DC 0 

* model definitions:
.model cmod_osdi capacitor_va c=5e-12

*OSDI Capacitor:
*OSDI_ACTIVATE*A1 D 0 cmod_osdi

*Built-in Capacitor:
*BUILT_IN_ACTIVATE*C1 D 0 5e-12


.control
pre_osdi capacitor.osdi
set filetype=ascii
set wr_vecnames
set wr_singlescale

* a DC sweep from 0.3V to 1V
dc Vd 0.3 1.0 0.01
wrdata dc_sim.ngspice v(d) i(vsense) 

* an AC sweep at Vd=0.5V
alter VD=0.5
ac dec 10 .01 10
wrdata ac_sim.ngspice v(d) i(vsense)

* a transient analysis
tran 100ms 500000ms 
wrdata tr_sim.ngspice v(d) i(vsense)

* print number of iterations
rusage totiter

.endc

.end