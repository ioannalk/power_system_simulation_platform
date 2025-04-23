#include "modules.h"

void history_currents (int (&Vbranch) [branches], int (&Ibranch) [branches], int (&Ihistory) [branches]) {
	
#pragma HLS inline off
#pragma HLS expression_balance
//#pragma HLS allocation operation instances=mul limit=

    int64_t tmp;

compute_history_currents:
	
    for (int i = 0; i < branches; i++) {
    #pragma HLS unroll
	
        tmp = (int64_t) Geff_voltage[i] * Vbranch[i] + (int64_t) Geff_current[i] * Ibranch[i];
		
        if (i < branches - 1 && Geff_transf[i] != 0) {
            tmp -= (int64_t) Geff_transf[i] * Vbranch[i + 1];
        }
		
        Ihistory[i] = tmp >> f1;
    }
}

