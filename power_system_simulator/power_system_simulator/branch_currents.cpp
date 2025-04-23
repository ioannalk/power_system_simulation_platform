#include "modules.h"

void branch_currents (int (&Vbranch) [branches], int (&Ihistory) [branches], int (&Ibranch) [branches]) {

#pragma HLS inline off
#pragma HLS expression_balance
//#pragma HLS allocation operation instances=mul limit=

    int64_t tmp;
	
compute_branch_currents:
	
    for (int i = 0; i < branches; i++) {
    #pragma HLS unroll
	
        tmp = (int64_t) Gbranch[i] * Vbranch[i] ; 
		
        if (i < branches - 1 && Gbranch_transf[i] != 0) {
            tmp -= (int64_t) Gbranch_transf[i] * Vbranch[i + 1];
        }
		
        Ibranch[i] = Ihistory[i] + (tmp >> f1);
    }
}

