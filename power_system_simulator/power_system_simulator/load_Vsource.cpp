#include "modules.h"

#ifndef current_sources

void load_Vsource (int (&Vsource) [voltage_src], int (&local_Vsource) [voltage_src]) {
	
#pragma HLS inline

load_Vsource:

    for (int i = 0; i < voltage_src; i++) {
    #pragma HLS unroll

        local_Vsource[i] = Vsource[i];
    }
}

#endif


