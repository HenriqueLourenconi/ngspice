#include "ngspice/config.h"

#include "ngspice/devdefs.h"

#include "numditf.h"
#include "numdext.h"
#include "numdinit.h"


SPICEdev NUMDinfo = {
    .DEVpublic = {
        .name = "NUMD",
        .description = "1D Numerical Junction Diode model",
        .terms = &NUMDnSize,
        .numNames = &NUMDnSize,
        .termNames = NUMDnames,
        .numInstanceParms = &NUMDpTSize,
        .instanceParms = NUMDpTable,
        .numModelParms = &NUMDmPTSize,
        .modelParms = NUMDmPTable,
#ifdef XSPICE
        .cm_func = NULL,
        .num_conn = 0,
        .conn = NULL,
        .num_param = 0,
        .param = NULL,
        .num_inst_var = 0,
        .inst_var = NULL,
#endif
        .flags = DEV_DEFAULT,
    },
    .DEVparam = NUMDparam,
    .DEVmodParam = NUMDmParam,
    .DEVload = NUMDload,
    .DEVsetup = NUMDsetup,
    .DEVunsetup = NULL,
    .DEVpzSetup = NUMDsetup,
    .DEVtemperature = NUMDtemp,
    .DEVtrunc = NUMDtrunc,
    .DEVfindBranch = NULL,
    .DEVacLoad = NUMDacLoad,
    .DEVaccept = NULL,
    .DEVdestroy = NUMDdestroy,
    .DEVmodDelete = NUMDmDelete,
    .DEVdelete = NUMDdelete,
    .DEVsetic = NULL,
    .DEVask = NUMDask,
    .DEVmodAsk = NULL,
    .DEVpzLoad = NUMDpzLoad,
    .DEVconvTest = NULL,
    .DEVsenSetup = NULL,
    .DEVsenLoad = NULL,
    .DEVsenUpdate = NULL,
    .DEVsenAcLoad = NULL,
    .DEVsenPrint = NULL,
    .DEVsenTrunc = NULL,
    .DEVdisto = NULL,
    .DEVnoise = NULL,
    .DEVsoaCheck = NULL,
#ifdef CIDER
    .DEVdump = NUMDdump,
    .DEVacct = NUMDacct,
#endif
    .DEVinstSize = &NUMDiSize,
    .DEVmodSize = &NUMDmSize,
};


SPICEdev *
get_numd_info(void)
{
    return &NUMDinfo;
}
