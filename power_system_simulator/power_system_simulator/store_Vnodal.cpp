#include "modules.h"

void store_Vnodal (int (&reg_Vnodal) [nodes], int (&Vnodal) [nodes_adj]) {
	
#pragma HLS inline

store_Vnodal:

    for (int i = 0; i < nodes; i++) {
    #pragma HLS unroll
	
        Vnodal[i] = reg_Vnodal[i];
    }
	
store_Vnodal_adj:

    for (int i = nodes; i < nodes_adj; i++) {
    #pragma HLS unroll
	
        Vnodal[i] = 0;
    }
}

