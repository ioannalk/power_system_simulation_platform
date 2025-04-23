#include "modules.h"

void branch_voltages (int (&Vnodal) [nodes], int (&Vbranch) [branches]) {

#pragma HLS inline off
#pragma HLS expression_balance

compute_branch_voltages:

    for (int i = 0; i < branches; i++) {
    #pragma HLS unroll
	
        if (BrnInfo_p[i] != 0 && BrnInfo_n[i] != 0) {
            Vbranch[i] = Vnodal[BrnInfo_p[i] - 1] - Vnodal[BrnInfo_n[i] - 1];
        }
        if (BrnInfo_p[i] == 0) {
            Vbranch[i] = - Vnodal[BrnInfo_n[i] - 1];
        }
        if (BrnInfo_n[i] == 0) {
            Vbranch[i] = Vnodal[BrnInfo_p[i] - 1];
        }
    }
}

