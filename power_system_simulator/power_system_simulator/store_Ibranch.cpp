#include "modules.h"

void store_Ibranch (int (&reg_Ibranch) [branches], int (&Ibranch) [branches_adj]) {

#pragma HLS inline

store_Ibranch:

    for (int i = 0; i < branches; i++) {
    #pragma HLS unroll
	
        Ibranch[i] = reg_Ibranch[i];
    }
	
store_Ibranch_adj:

    for (int i = branches; i < branches_adj; i++) {
    #pragma HLS unroll

        Ibranch[i] = 0;
    }
}

