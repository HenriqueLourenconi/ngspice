/*
 * Copyright (c) 2014, NVIDIA Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation and/or 
 *    other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to 
 *    endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ngspice/config.h"
#include "ngspice/cktdefs.h"
#include "ngspice/sperror.h"
#include "cuda_runtime_api.h"
#include "ngspice/CUSPICE/CUSPICE.h"

/* cudaMemcpy MACRO to check it for errors --> CUDAMEMCPYCHECK(name of pointer, dimension, type, status) */
#define CUDAMEMCPYCHECK(a, b, c, d) \
    if (d != cudaSuccess) \
    { \
        fprintf (stderr, "cuCKTsystem routine...\n") ; \
        fprintf (stderr, "Error: cudaMemcpy failed on %s size of %d bytes\n", #a, (int)(b * sizeof(c))) ; \
        fprintf (stderr, "Error: %s = %d, %s\n", #d, d, cudaGetErrorString (d)) ; \
        return (E_NOMEM) ; \
    }

int
cuCKTsystemDtoH
(
CKTcircuit *ckt
)
{
    long unsigned int nz, n ;
    cudaError_t status ;

    if (ckt->total_n_Ptr > 0) {
        nz = (long unsigned int)ckt->CKTmatrix->CKTklunz ;

        /* Copy back the Matrix */
        status = cudaMemcpy (ckt->CKTmatrix->CKTkluAx, ckt->CKTmatrix->d_CKTkluAx, nz * sizeof(double), cudaMemcpyDeviceToHost) ;
        CUDAMEMCPYCHECK (ckt->CKTmatrix->CKTkluAx, nz, double, status)
    }

    if (ckt->total_n_PtrRHS > 0) {
        n = (long unsigned int)ckt->CKTmatrix->CKTkluN ;

        /* Copy back the RHS */
        status = cudaMemcpy (ckt->CKTrhs, ckt->CKTmatrix->d_CKTrhs, (n + 1) * sizeof(double), cudaMemcpyDeviceToHost) ;
        CUDAMEMCPYCHECK (ckt->CKTrhs, (n + 1), double, status)
    }

    return (OK) ;
}
