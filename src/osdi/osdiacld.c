/*
 * Copyright© 2022 SemiMod UG. All rights reserved.
 */

#include "ngspice/iferrmsg.h"
#include "ngspice/memory.h"
#include "ngspice/ngspice.h"
#include "ngspice/typedefs.h"

#include "osdi.h"
#include "osdidefs.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int OSDIacLoad(GENmodel *inModel, CKTcircuit *ckt) {

  GENmodel *gen_model;
  GENinstance *gen_inst;

  OsdiRegistryEntry *entry = osdi_reg_entry_model(inModel);
  const OsdiDescriptor *descr = entry->descriptor;
  for (gen_model = inModel; gen_model; gen_model = gen_model->GENnextModel) {
    void *model = osdi_model_data(gen_model);

    for (gen_inst = gen_model->GENinstances; gen_inst;
         gen_inst = gen_inst->GENnextInstance) {
      void *inst = osdi_instance_data(entry, gen_inst);
      // nothing to calculate just load the matrix entries calculated during
      // operating point iterations
      descr->load_jacobian_resist(inst, model);
      descr->load_jacobian_react(inst, model, ckt->CKTomega);
    }
  }
  return (OK);
}
