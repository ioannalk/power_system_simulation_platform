#include "modules.h"

#ifndef voltage_sources

void load_Isource (int (&Isource) [current_src], int (&local_Isource) [current_src]) {

#pragma HLS inline

load_Isource:

    for (int i = 0; i < current_src; i++) {
    #pragma HLS unroll

        local_Isource[i] = Isource[i];
    }
}
	
#endif


