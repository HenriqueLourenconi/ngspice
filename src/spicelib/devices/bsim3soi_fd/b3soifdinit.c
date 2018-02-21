#include "ngspice/config.h"

#include "ngspice/devdefs.h"

#include "b3soifditf.h"
#include "b3soifdinit.h"

SPICEdev B3SOIFDinfo = {
    .DEVpublic = {
        .name = "B3SOIFD",
        .description = "Berkeley SOI MOSFET (FD) model version 2.1",
        .terms = &B3SOIFDnSize,
        .numNames = &B3SOIFDnSize,
        .termNames = B3SOIFDnames,
        .numInstanceParms = &B3SOIFDpTSize,
        .instanceParms = B3SOIFDpTable,
        .numModelParms = &B3SOIFDmPTSize,
        .modelParms = B3SOIFDmPTable,
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
    .DEVparam = B3SOIFDparam,
    .DEVmodParam = B3SOIFDmParam,
    .DEVload = B3SOIFDload,
    .DEVsetup = B3SOIFDsetup,
    .DEVunsetup = B3SOIFDunsetup,
    .DEVpzSetup = B3SOIFDsetup,
    .DEVtemperature = B3SOIFDtemp,
    .DEVtrunc = B3SOIFDtrunc,
    .DEVfindBranch = NULL,
    .DEVacLoad = B3SOIFDacLoad,
    .DEVaccept = NULL,
    .DEVdestroy = B3SOIFDdestroy,
    .DEVmodDelete = B3SOIFDmDelete,
    .DEVdelete = B3SOIFDdelete,
    .DEVsetic = B3SOIFDgetic,
    .DEVask = B3SOIFDask,
    .DEVmodAsk = B3SOIFDmAsk,
    .DEVpzLoad = B3SOIFDpzLoad,
    .DEVconvTest = B3SOIFDconvTest,
    .DEVsenSetup = NULL,
    .DEVsenLoad = NULL,
    .DEVsenUpdate = NULL,
    .DEVsenAcLoad = NULL,
    .DEVsenPrint = NULL,
    .DEVsenTrunc = NULL,
    .DEVdisto = NULL,
    .DEVnoise = B3SOIFDnoise,
    .DEVsoaCheck = NULL,
#ifdef CIDER
    .DEVdump = NULL,
    .DEVacct = NULL,
#endif
    .DEVinstSize = &B3SOIFDiSize,
    .DEVmodSize = &B3SOIFDmSize,
};






SPICEdev *
get_b3soifd_info (void)
{
  return &B3SOIFDinfo;
}
