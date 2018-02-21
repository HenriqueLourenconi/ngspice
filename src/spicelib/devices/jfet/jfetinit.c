#include "ngspice/config.h"

#include "ngspice/devdefs.h"

#include "jfetitf.h"
#include "jfetext.h"
#include "jfetinit.h"


SPICEdev JFETinfo = {
    .DEVpublic = {
        .name = "JFET",
        .description = "Junction Field effect transistor",
        .terms = &JFETnSize,
        .numNames = &JFETnSize,
        .termNames = JFETnames,
        .numInstanceParms = &JFETpTSize,
        .instanceParms = JFETpTable,
        .numModelParms = &JFETmPTSize,
        .modelParms = JFETmPTable,
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
    .DEVparam = JFETparam,
    .DEVmodParam = JFETmParam,
    .DEVload = JFETload,
    .DEVsetup = JFETsetup,
    .DEVunsetup = JFETunsetup,
    .DEVpzSetup = JFETsetup,
    .DEVtemperature = JFETtemp,
    .DEVtrunc = JFETtrunc,
    .DEVfindBranch = NULL,
    .DEVacLoad = JFETacLoad,
    .DEVaccept = NULL,
    .DEVdestroy = JFETdestroy,
    .DEVmodDelete = JFETmDelete,
    .DEVdelete = JFETdelete,
    .DEVsetic = JFETgetic,
    .DEVask = JFETask,
    .DEVmodAsk = JFETmAsk,
    .DEVpzLoad = JFETpzLoad,
    .DEVconvTest = NULL,
    .DEVsenSetup = NULL,
    .DEVsenLoad = NULL,
    .DEVsenUpdate = NULL,
    .DEVsenAcLoad = NULL,
    .DEVsenPrint = NULL,
    .DEVsenTrunc = NULL,
    .DEVdisto = JFETdisto,
    .DEVnoise = JFETnoise,
    .DEVsoaCheck = NULL,
#ifdef CIDER
    .DEVdump = NULL,
    .DEVacct = NULL,
#endif
    .DEVinstSize = &JFETiSize,
    .DEVmodSize = &JFETmSize,
};


SPICEdev *
get_jfet_info(void)
{
    return &JFETinfo;
}
