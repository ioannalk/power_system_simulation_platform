#include "power_system_simulator.h"
#include "initial_state.h"


#ifdef voltage_sources

void power_system_simulator (int (&Vsource) [voltage_src], int (&Vnodal) [nodes_adj], int (&Ibranch) [branches_adj]) {
	
#pragma HLS expression_balance

#pragma HLS reset variable=reg_Ibranch
#pragma HLS array_partition variable=reg_Ibranch complete dim=1
#pragma HLS reset variable=reg_Vbranch
#pragma HLS array_partition variable=reg_Vbranch complete dim=1
#pragma HLS reset variable=reg_Vnodal
#pragma HLS array_partition variable=reg_Vnodal complete dim=1

#pragma HLS interface m_axi port=Vsource depth=voltage_src offset=slave bundle=v_source
#pragma HLS interface m_axi port=Vnodal depth=nodes_adj offset=slave bundle=v_nondal
#pragma HLS interface m_axi port=Ibranch depth=branches_adj offset=slave bundle=i_branch

#pragma HLS interface s_axilite port=Vsource 
#pragma HLS interface s_axilite port=Vnodal 
#pragma HLS interface s_axilite port=Ibranch
#pragma HLS interface s_axilite port=return


    int Inodal[nodes - voltage_src];
#pragma HLS array_partition variable=Inodal complete dim=1
    int Ihistory[branches];
#pragma HLS array_partition variable=Ihistory complete dim=1
    int local_Vsource[voltage_src];
#pragma HLS array_partition variable=local_Vsource complete dim=1

    load_Vsource (Vsource, local_Vsource);
	
    store_Vnodal (reg_Vnodal, Vnodal);
    store_Ibranch (reg_Ibranch, Ibranch);
		
    history_currents (reg_Vbranch, reg_Ibranch, Ihistory);
    nodal_currents (local_Vsource, Ihistory, Inodal);
    nodal_voltages (local_Vsource, Inodal, reg_Vnodal);
    branch_voltages (reg_Vnodal, reg_Vbranch);
    branch_currents (reg_Vbranch, Ihistory, reg_Ibranch);
		
}

#endif



#ifdef current_sources

void power_system_simulator (int (&Isource) [current_src], int (&Vnodal) [nodes_adj], int (&Ibranch) [branches_adj]) {
	
#pragma HLS expression_balance

#pragma HLS reset variable=reg_Ibranch
#pragma HLS array_partition variable=reg_Ibranch complete dim=1
#pragma HLS reset variable=reg_Vbranch
#pragma HLS array_partition variable=reg_Vbranch complete dim=1
#pragma HLS reset variable=reg_Vnodal
#pragma HLS array_partition variable=reg_Vnodal complete dim=1

#pragma HLS interface m_axi port=Isource depth=current_src offset=slave bundle=i_source
#pragma HLS interface m_axi port=Vnodal depth=nodes_adj offset=slave bundle=v_nondal
#pragma HLS interface m_axi port=Ibranch depth=branches_adj offset=slave bundle=i_branch

#pragma HLS interface s_axilite port=Isource 
#pragma HLS interface s_axilite port=Vnodal 
#pragma HLS interface s_axilite port=Ibranch
#pragma HLS interface s_axilite port=return


    int Inodal[nodes];
#pragma HLS array_partition variable=Inodal complete dim=1
    int Ihistory[branches];
#pragma HLS array_partition variable=Ihistory complete dim=1
    int local_Isource[current_src];
#pragma HLS array_partition variable=local_Isource complete dim=1

    load_Isource (Isource, local_Isource);
	
    store_Vnodal (reg_Vnodal, Vnodal);
    store_Ibranch (reg_Ibranch, Ibranch);

    history_currents (reg_Vbranch, reg_Ibranch, Ihistory);
    nodal_currents (local_Isource, Ihistory, Inodal);
    nodal_voltages (Inodal, reg_Vnodal);
    branch_voltages (reg_Vnodal, reg_Vbranch);
    branch_currents (reg_Vbranch, Ihistory, reg_Ibranch);
		
}

#endif



#ifdef voltage_and_current_sources

void power_system_simulator (int (&Vsource) [voltage_src], int (&Isource) [current_src], int (&Vnodal) [nodes_adj], int (&Ibranch) [branches_adj]) {
	
#pragma HLS expression_balance

#pragma HLS reset variable=reg_Ibranch
#pragma HLS array_partition variable=reg_Ibranch complete dim=1
#pragma HLS reset variable=reg_Vbranch
#pragma HLS array_partition variable=reg_Vbranch complete dim=1
#pragma HLS reset variable=reg_Vnodal
#pragma HLS array_partition variable=reg_Vnodal complete dim=1

#pragma HLS interface m_axi port=Vsource depth=voltage_src offset=slave bundle=v_source
#pragma HLS interface m_axi port=Isource depth=current_src offset=slave bundle=i_source
#pragma HLS interface m_axi port=Vnodal depth=nodes_adj offset=slave bundle=v_nondal
#pragma HLS interface m_axi port=Ibranch depth=branches_adj offset=slave bundle=i_branch

#pragma HLS interface s_axilite port=Vsource 
#pragma HLS interface s_axilite port=Isource 
#pragma HLS interface s_axilite port=Vnodal 
#pragma HLS interface s_axilite port=Ibranch
#pragma HLS interface s_axilite port=return


    int Inodal[nodes - voltage_src];
#pragma HLS array_partition variable=Inodal complete dim=1
    int Ihistory[branches];
#pragma HLS array_partition variable=Ihistory complete dim=1
    int local_Vsource[voltage_src];
#pragma HLS array_partition variable=local_Vsource complete dim=1
    int local_Isource[current_src];
#pragma HLS array_partition variable=local_Isource complete dim=1

    load_Vsource (Vsource, local_Vsource);
    load_Isource (Isource, local_Isource);
	
    store_Vnodal (reg_Vnodal, Vnodal);
    store_Ibranch (reg_Ibranch, Ibranch);

    history_currents (reg_Vbranch, reg_Ibranch, Ihistory);
    nodal_currents (local_Vsource, local_Isource, Ihistory, Inodal);
    nodal_voltages (local_Vsource, Inodal, reg_Vnodal);
    branch_voltages (reg_Vnodal, reg_Vbranch);
    branch_currents (reg_Vbranch, Ihistory, reg_Ibranch);
		
}

#endif
