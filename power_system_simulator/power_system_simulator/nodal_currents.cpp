#include "modules.h"

#ifdef current_sources

void nodal_currents (int (&Isource) [current_src], int (&Ihistory) [branches], int (&Inodal) [nodes]) {

#pragma HLS inline off
#pragma HLS expression_balance 

    int tmp;
    int64_t tmp_Ihistory;

compute_nodal_currents:
	
    for (int i = 1; i < nodes + 1; i++) {
    #pragma HLS unroll
	
        tmp = 0;

compute_nodal_currents_Ihistory_term:
			
        for (int j = 0; j < branches; j++) {
        #pragma HLS unroll 
			
            if (BrnInfo_p[j] == i) {
					
                if (j != 0) {
                    tmp_Ihistory = (int64_t) Ihistory[j - 1] * Transf[j - 1];
                    tmp += tmp_Ihistory >> f2;
                }
					
                tmp -= Ihistory[j];
            }
            else if (BrnInfo_n[j] == i) {
					
                if (j != 0) {
                    tmp_Ihistory = (int64_t) Ihistory[j - 1] * Transf[j - 1];
                    tmp -= tmp_Ihistory >> f2;
                }
					
                tmp += Ihistory[j];
            } 
        }

compute_nodal_currents_Isource_term:
			
        for (int j = 0; j < current_src; j++) {
        #pragma HLS unroll
			
            if (IsInfo_p[j] == i) {
                tmp -= Isource[j];
            }
            if (IsInfo_n[j] == i) {
                tmp += Isource[j];
            }
        }
			
        Inodal[i - 1] = tmp;
    }
}
	
#endif


#ifdef voltage_sources

void nodal_currents (int (&Vsource) [voltage_src], int (&Ihistory) [branches], int (&Inodal) [nodes - voltage_src]) {

#pragma HLS inline off
#pragma HLS expression_balance

    int tmp;
    int64_t tmp_Ihistory;
    int64_t tmp_Iunknown;
    int unknown = 0;
    int known;

compute_nodal_currents:
	
    for (int i = 1; i < nodes + 1; i++) {
    #pragma HLS unroll
	
        known = 0;
		
compute_nodal_currents_exclude_Iknown:
		
        for (int j = 0; j < voltage_src; j++) {
        #pragma HLS unroll 
		
            if (VsInfo_p[j] == i || VsInfo_n[j] == i) {
                known = 1;
            }
        }
		
        if (known == 0) {
		
            tmp = 0;

compute_nodal_currents_Ihistory_term:
			
            for (int j = 0; j < branches; j++) {
            #pragma HLS unroll 
			
                if (BrnInfo_p[j] == i) {
					
                    if (j != 0) {
                        tmp_Ihistory = (int64_t) Ihistory[j - 1] * Transf[j - 1];
                        tmp += tmp_Ihistory >> f2;
                    }
					
                    tmp -= Ihistory[j];
                }
                else if (BrnInfo_n[j] == i) {
					
                    if (j != 0) {
                        tmp_Ihistory = (int64_t) Ihistory[j - 1] * Transf[j - 1];
                        tmp -= tmp_Ihistory >> f2;
                    }
					
                    tmp += Ihistory[j];
                } 
            }
			
            tmp_Iunknown = 0;
			
compute_nodal_currents_Vknown_term:
			
            for (int j = 0; j < voltage_src; j++) {
            #pragma HLS unroll 
			
                if (VsInfo_p[j] != 0) {
                    tmp_Iunknown += (int64_t) G_UK[unknown][j] * Vsource[j];
                }
                else if (VsInfo_n[j] != 0) {
                    tmp_Iunknown -= (int64_t) G_UK[unknown][j] * Vsource[j];
                }
            }

            Inodal[unknown] = tmp - ( tmp_Iunknown >> f1 );
			
            unknown++;
        }
    }
}
	
#endif


#ifdef voltage_and_current_sources

void nodal_currents (int (&Vsource) [voltage_src], int (&Isource) [current_src], int (&Ihistory) [branches], int (&Inodal) [nodes - voltage_src]) {

#pragma HLS inline off
#pragma HLS expression_balance 

    int tmp;
    int64_t tmp_Ihistory;
    int64_t tmp_Iunknown;
    int unknown = 0;
    int known;
	
compute_nodal_currents:
	
    for (int i = 1; i < nodes + 1; i++) {
    #pragma HLS unroll
	
        known = 0;
		
compute_nodal_currents_exclude_Iknown:
		
        for (int j = 0; j < voltage_src; j++) {
        #pragma HLS unroll 
		
            if (VsInfo_p[j] == i || VsInfo_n[j] == i) {
                known = 1;
            }
        }
		
        if (known == 0) {
		
            tmp = 0;
			
compute_nodal_currents_Ihistory_term:
			
            for (int j = 0; j < branches; j++) {
            #pragma HLS unroll 
			
                if (BrnInfo_p[j] == i) {
					
                    if (j != 0) {
                        tmp_Ihistory = (int64_t) Ihistory[j - 1] * Transf[j - 1];
                        tmp += tmp_Ihistory >> f2;
                    }
					
                    tmp -= Ihistory[j];
                }
                else if (BrnInfo_n[j] == i) {
					
                    if (j != 0) {
                        tmp_Ihistory = (int64_t) Ihistory[j - 1] * Transf[j - 1];
                        tmp -= tmp_Ihistory >> f2;
                    }
					
                    tmp += Ihistory[j];
                } 
            }
			
compute_nodal_currents_Isource_term:
			
            for (int j = 0; j < current_src; j++) {
            #pragma HLS unroll
			
                if (IsInfo_p[j] == i) {
                    tmp -= Isource[j];
                }
                if (IsInfo_n[j] == i) {
                    tmp += Isource[j];
                }
            }
			
            tmp_Iunknown = 0;
			
compute_nodal_currents_Vknown_term:
	
            for (int j = 0; j < voltage_src; j++) {
            #pragma HLS unroll 
			
                if (VsInfo_p[j] != 0) {
                    tmp_Iunknown += (int64_t) G_UK[unknown][j] * Vsource[j];
                }
                else if (VsInfo_n[j] != 0) {
                    tmp_Iunknown -= (int64_t) G_UK[unknown][j] * Vsource[j];
                }
            }

            Inodal[unknown] = tmp - ( tmp_Iunknown >> f1 );
			
            unknown++;
        }
    }
}
	
#endif
