#include "ngspice/config.h"

#include "ngspice/devdefs.h"

#include "ccvsitf.h"
#include "ccvsext.h"
#include "ccvsinit.h"


SPICEdev CCVSinfo = {
    .DEVpublic = {
        .name = "CCVS",
        .description = "Linear current controlled current source",
        .terms = &CCVSnSize,
        .numNames = &CCVSnSize,
        .termNames = CCVSnames,
        .numInstanceParms = &CCVSpTSize,
        .instanceParms = CCVSpTable,
        .numModelParms = NULL,
        .modelParms = NULL,
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
    .DEVparam = CCVSparam,
    .DEVmodParam = NULL,
    .DEVload = CCVSload,
    .DEVsetup = CCVSsetup,
    .DEVunsetup = CCVSunsetup,
    .DEVpzSetup = CCVSsetup,
    .DEVtemperature = NULL,
    .DEVtrunc = NULL,
    .DEVfindBranch = CCVSfindBr,
    .DEVacLoad = CCVSload,
    .DEVaccept = NULL,
    .DEVdestroy = CCVSdestroy,
    .DEVmodDelete = CCVSmDelete,
    .DEVdelete = CCVSdelete,
    .DEVsetic = NULL,
    .DEVask = CCVSask,
    .DEVmodAsk = NULL,
    .DEVpzLoad = CCVSpzLoad,
    .DEVconvTest = NULL,
    .DEVsenSetup = CCVSsSetup,
    .DEVsenLoad = CCVSsLoad,
    .DEVsenUpdate = NULL,
    .DEVsenAcLoad = CCVSsAcLoad,
    .DEVsenPrint = CCVSsPrint,
    .DEVsenTrunc = NULL,
    .DEVdisto = NULL,
    .DEVnoise = NULL,
    .DEVsoaCheck = NULL,
#ifdef CIDER
    .DEVdump = NULL,
    .DEVacct = NULL,
#endif
    .DEVinstSize = &CCVSiSize,
    .DEVmodSize = &CCVSmSize,
};


SPICEdev *
get_ccvs_info(void)
{
    return &CCVSinfo;
}
