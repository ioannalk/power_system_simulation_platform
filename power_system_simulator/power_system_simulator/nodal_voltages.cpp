#include "modules.h"

#if defined(current_sources) && defined(nodal_voltages_full_unroll) 
		
void nodal_voltages (int (&Inodal) [nodes], int (&Vnodal) [nodes]) {

#pragma HLS inline off
#pragma HLS expression_balance
#pragma HLS array_partition variable=G complete dim=1
#pragma HLS array_partition variable=G complete dim=2
//#pragma HLS allocation operation instances=mul limit=

    int64_t tmp;
			
compute_nodal_voltages:

    for (int i = 0; i < nodes; i++) {
    #pragma HLS unroll
	
        tmp = 0;
				
compute_nodal_voltages_G_term:

        for (int j = 0; j < nodes; j++) {		
        #pragma HLS unroll 	
		
            tmp += (int64_t) G[i][j] * Inodal[j];
        }
		
        Vnodal[i] = tmp >> f2;
    }
}
	
#endif


#if !defined(current_sources) && defined(nodal_voltages_full_unroll) 
		
void nodal_voltages (int (&Vsource) [voltage_src], int (&Inodal) [nodes - voltage_src], int (&Vnodal) [nodes]) {
	
#pragma HLS inline off
#pragma HLS expression_balance
#pragma HLS array_partition variable=G_UU complete dim=1
#pragma HLS array_partition variable=G_UU complete dim=2
//#pragma HLS allocation operation instances=mul limit=

    int64_t tmp;
    int unknown = 0;
    int known;
			
compute_nodal_voltages:

    for (int i = 1; i < nodes + 1; i++) {
    #pragma HLS unroll

        known = 0;
				
compute_nodal_voltages_Vknown:
		
        for (int j = 0; j < voltage_src; j++) {
        #pragma HLS unroll 
		
            if (VsInfo_p[j] == i) {
                Vnodal[i - 1] = Vsource[j];
                known = 1;
            }
            if (VsInfo_n[j] == i) {
                Vnodal[i - 1] = - Vsource[j];
                known = 1;
            }
        }
		
        if (known == 0) {
			
            tmp = 0;
					
compute_nodal_voltages_GUU_term:
		
            for (int j = 0; j < nodes - voltage_src; j++) {
            #pragma HLS unroll
			
                tmp += (int64_t) G_UU[unknown][j] * Inodal[j];
            }
			
            Vnodal[i - 1] = tmp >> f2;
			
            unknown++;
        }		
    }
}

#endif	

	
#if defined(current_sources) && defined(nodal_voltages_partial_unroll) 
		
void nodal_voltages (int (&Inodal) [nodes], int (&Vnodal) [nodes]) {
	
#pragma HLS inline off
#pragma HLS expression_balance
#pragma HLS array_partition variable=G complete dim=2

    int64_t tmp;
			
compute_nodal_voltages:

    for (int i = 0; i < nodes_even; i++) {
    #pragma HLS loop_tripcount min=nodes_even max=nodes_even
    #pragma HLS unroll factor=unroll_factor
    #pragma HLS pipeline II=1
	
        if (i < nodes) {
		
            tmp = 0;
				
compute_nodal_voltages_G_term:

            for (int j = 0; j < nodes; j++) {		
            #pragma HLS unroll 	
		
                tmp += (int64_t) G[i][j] * Inodal[j];
            }
		
            Vnodal[i] = tmp >> f2;
        }
    }
}
	
#endif
	
	
#if !defined(current_sources) && defined(nodal_voltages_partial_unroll) 	
	
void nodal_voltages (int (&Vsource) [voltage_src], int (&Inodal) [nodes - voltage_src], int (&Vnodal) [nodes]) {

#pragma HLS inline off
#pragma HLS expression_balance
#pragma HLS array_partition variable=G_UU complete dim=2

    int64_t tmp;
    int unknown = 0;
    int known;
			
compute_nodal_voltages:

    for (int i = 1; i < nodes_even + 1; i++) {
    #pragma HLS loop_tripcount min=nodes_even max=nodes_even
    #pragma HLS unroll factor=unroll_factor
    #pragma HLS pipeline II=1

        if (i < nodes + 1) {
	
            known = 0;
				
compute_nodal_voltages_Vknown:
		
            for (int j = 0; j < voltage_src; j++) {
            #pragma HLS unroll 
		
                if (VsInfo_p[j] == i) {
                    Vnodal[i - 1] = Vsource[j];
                    known = 1;
                }
                if (VsInfo_n[j] == i) {
                    Vnodal[i - 1] = - Vsource[j];
                    known = 1;
                }
            }
		
            if (known == 0) {
			
                tmp = 0;
					
compute_nodal_voltages_GUU_term:
		
                for (int j = 0; j < nodes - voltage_src; j++) {
                #pragma HLS unroll
			
                    tmp += (int64_t) G_UU[unknown][j] * Inodal[j];
                }
			
                Vnodal[i - 1] = tmp >> f2;
			
	        unknown++;
            }	
        }			
    }
}

#endif


#if defined(current_sources) && defined(nodal_voltages_pipelined) 
		
void nodal_voltages (int (&Inodal) [nodes], int (&Vnodal) [nodes]) {

#pragma HLS inline off
#pragma HLS expression_balance
#pragma HLS array_partition variable=G complete dim=2

    int64_t tmp;
			
compute_nodal_voltages:

    for (int i = 0; i < nodes; i++) {
    #pragma HLS pipeline II=1 
	
        tmp = 0;
				
compute_nodal_voltages_G_term:

        for (int j = 0; j < nodes; j++) {		
        #pragma HLS unroll 	
		
            tmp += (int64_t) G[i][j] * Inodal[j];
        }
		
        Vnodal[i] = tmp >> f2;
    }
}
	
#endif


#if !defined(current_sources) && defined(nodal_voltages_pipelined) 
		
void nodal_voltages (int (&Vsource) [voltage_src], int (&Inodal) [nodes - voltage_src], int (&Vnodal) [nodes]) {
	
#pragma HLS inline off
#pragma HLS expression_balance
#pragma HLS array_partition variable=G_UU complete dim=2

    int64_t tmp;
    int unknown = 0;
    int known;
			
compute_nodal_voltages:

    for (int i = 1; i < nodes + 1; i++) {
    #pragma HLS pipeline II=1

        known = 0;
				
compute_nodal_voltages_Vknown:
		
        for (int j = 0; j < voltage_src; j++) {
        #pragma HLS unroll 
		
            if (VsInfo_p[j] == i) {
                Vnodal[i - 1] = Vsource[j];
                known = 1;
            }
            if (VsInfo_n[j] == i) {
                Vnodal[i - 1] = - Vsource[j];
                known = 1;
            }
        }
		
        if (known == 0) {
			
            tmp = 0;
					
compute_nodal_voltages_GUU_term:
		
            for (int j = 0; j < nodes - voltage_src; j++) {
            #pragma HLS unroll
			
                tmp += (int64_t) G_UU[unknown][j] * Inodal[j];
            }
			
            Vnodal[i - 1] = tmp >> f2;
			
            unknown++;
        }		
    }
}

#endif	
