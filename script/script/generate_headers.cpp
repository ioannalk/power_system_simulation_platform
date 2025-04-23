#include <fstream>
#include <string>
#include <iostream>

//***************************************************************************************************************************************//
//this script reads the txt files exported by Algorithm 2 in Simulink to generate 4 header files//
//constants.h, initial_state.h, input_sources.h, ps_sim_host.h//
//constants.h contains all necessary information about the system - used for HLS implementation//
//initial_state.h contains the initial state of the system (voltages and currents) in t = 0 - used for HLS implementation//
//input_sources.h contains the number of samples and the sources feeding the system - used by both HLS and host application//
//ps_sim_host.h contains power system size and type of sources - used by host application//
//***************************************************************************************************************************************//
//3 different architectures depending on the kind of sources: voltage sources, current sources, voltage and current sources//
//3 different architectures depending on the number of nodes: full unroll, unroll with a factor, only pipeline//
//the number of nodes defines which architecture will be used - the module affected is nodal_voltages //
//the limits of each architecture may be changed by the user//
//***************************************************************************************************************************************//
//in case of architecture of PARTIAL_UNROLL, the factor likely needs to be defined by the user - default is 2, as the safest option//
//***************************************************************************************************************************************//

#define buswidth 128 //ps_ddr buswidth is 128 bits for Ultrascale+ devices (pl_ddr has usually a buswidth of 512 bits) 
 
#define limit_full_unroll 69 //adjust if needed
#define limit_partial_unroll 111 //adjust if needed

#define unroll_factor 2 //adjust as needed

using namespace std;

int main(void) {
	
    string string_read;
    ifstream file_read;
    ofstream file;
	
//Read coefficients from files//
	
    int coeffs[8];

    file_read.open ("coeffs.txt");
	
    if (file_read.is_open()) { 
        for (int i = 0; i < 8 && file_read >> string_read; i++) {
            coeffs[i] = stol(string_read,0,10); 
        }
        file_read.close();
    }
    else {
        cout << "Failed to open coeffs.txt\n";
        return 1;
    }

//Assign values from coeffs files//	

    int n = coeffs[0]; // number of bits - not used since bits = 32
    int f1 = coeffs[1]; // fraction length 1
    int f2 = coeffs[2]; // fraction length 2
    int nodes = coeffs[3]; // number of nodes
    int branches = coeffs[4]; // number of branches
    int src = coeffs[5]; // number of total sources
    int vk = coeffs[6]; // number of voltage sources
    int samples = coeffs[7]; // number of samples
	
//Adjusted dimensions for padding, m_axi bus = 128 bits (128bits / 32bits = 4)// 	

    int nodes_adj ;
    int branches_adj;
    int width = buswidth / 32;
	
    if (nodes % width == 0) {
        nodes_adj = nodes ;
    }
    else {
        nodes_adj = nodes + width - (nodes % width) ;
    }
	
    if (branches % width == 0) {
        branches_adj = branches ;
    }
    else {
        branches_adj = branches + width - (branches % width) ;
    }
	
//Adjusted number of nodes, in case of partial_unroll architecture// 	

    int nodes_even ;

    if (nodes % unroll_factor == 0) {
        nodes_even = nodes ;
    }
    else {
        nodes_even = nodes + unroll_factor - (nodes % unroll_factor) ;
    }
	
//Declaration of arrays to store data from files//

    int coeff1_ibranch [branches]; // G_br 
	
    int coeff_transf [branches]; // Transf
    int coeff3_ihistory [branches]; // Transf
    int coeff2_ibranch [branches]; // Transf
	
    int coeff1_branch_info [branches]; // BrnInfo
    int coeff2_branch_info [branches]; // BrnInfo
	
    int coeff1_ihistory [branches]; // I_hs
    int coeff2_ihistory [branches]; // I_hs
	
    int coeff1_is_info [src - vk]; // IsInfo
    int coeff2_is_info [src - vk]; // IsInfo
	
    int coeff1_vs_info [vk]; // VsInfo
    int coeff2_vs_info [vk]; // VsInfo
	
    int g [nodes * nodes]; // G
    int g_uu [(nodes - vk) * (nodes - vk)]; // G_UU
    int g_uk [(nodes - vk) * vk]; // G_UK
	
    int vnodal_initial [nodes]; // V_n
    int vbranch_initial [branches]; // V_branch
    int ibranch_initial [branches]; // I_br
	
    int vs [samples * vk]; // V_s
    int is [samples * (src - vk)]; //I_s
	
//Read and store data from files//

//Read G_br//

    file_read.open ("G_br.txt");
	
    if (file_read.is_open()) {
        for (int i = 0; i < branches && file_read >> string_read; i++) {		
            coeff1_ibranch[i] = stol(string_read,0,10);
        }
        file_read.close();
    }
    else {
        cout << "Failed to open G_br.txt\n";
        return 1;
    }

//Read Transf//	

    file_read.open ("Transf.txt");
	
    if (file_read.is_open()) {
        for (int i = 0; i < branches; i++) {
            if (file_read >> string_read) {
                coeff_transf[i] = stol(string_read,0,10);
            }
            if (file_read >> string_read) {
                coeff3_ihistory[i] = stol(string_read,0,10);
            }
            if (file_read >> string_read) {
                coeff2_ibranch[i] = stol(string_read,0,10);
            }
        }
        file_read.close();
    }
    else {
        cout << "Failed to open Transf.txt\n";
        return 1;
    }

//Read BrnInfo//

    file_read.open ("BrnInfo.txt");
	
    if (file_read.is_open()) {
        for (int i = 0; i < branches; i++) {
            if (file_read >> string_read) {
                coeff1_branch_info[i] = stol(string_read,0,10);
            }
            if (file_read >> string_read) {
                coeff2_branch_info[i] = stol(string_read,0,10);
            }
        }
        file_read.close();
    }
    else {
        cout << "Failed to open BrnInfo.txt\n";
        return 1;
    }

//Read I_hs//

    file_read.open ("I_hs.txt");
	
    if (file_read.is_open()) {
        for (int i = 0; i < branches; i++) {
            if (file_read >> string_read) {
                coeff1_ihistory[i] = stol(string_read,0,10);
            }
            if (file_read >> string_read) {
                coeff2_ihistory[i] = stol(string_read,0,10);
            }
        }
        file_read.close();
    }
    else {
        cout << "Failed to open I_hs.txt\n";
        return 1;
    }
	
//Read IsInfo - if there are current sources//	

    if (vk != src) {

        file_read.open ("IsInfo.txt");
		
        if( file_read.is_open()) {
            for (int i = 0; i < src - vk; i++) {
                if (file_read >> string_read) {
                    coeff1_is_info[i] = stol(string_read,0,10);
                }
                if (file_read >> string_read) {
                    coeff2_is_info[i] = stol(string_read,0,10);
                }
            }
            file_read.close();
        }
        else {
            cout << "Failed to open IsInfo.txt\n";
            return 1;
        }
    }

//Read VsInfo - if there are voltage sources//	
	
    if (vk != 0) {
		
        file_read.open ("VsInfo.txt");
		
        if( file_read.is_open()) {
            for (int i = 0; i < vk; i++) {
                if (file_read >> string_read) {
                    coeff1_vs_info[i] = stol(string_read,0,10);
                }
                if (file_read >> string_read) {
                    coeff2_vs_info[i] = stol(string_read,0,10);
                }
            }
            file_read.close();
        }
        else {
            cout << "Failed to open VsInfo.txt\n";
            return 1;
        }
    }
	
//Read G - if there are only current sources//	

    if (vk == 0) {
		
        file_read.open ("G.txt");
		
        if (file_read.is_open()) {
            for (int i = 0; i < nodes * nodes && file_read >> string_read; i++) {
                g[i] = stol(string_read,0,10);
            }
            file_read.close();
        }
        else {
            cout << "Failed to open G.txt\n";
            return 1;
        }
    }
	
//Read G_UK and G_UU - if there are voltage sources//	
	
    else {
				
        file_read.open ("G_UU.txt");
		
        if (file_read.is_open()) {
            for (int i = 0; i < (nodes - vk) * (nodes - vk) && file_read >> string_read; i++) {
                g_uu[i] = stol(string_read,0,10);
            }
            file_read.close();
        }
        else {
            cout << "Failed to open G_UU.txt\n";
            return 1;
        }
		
        file_read.open ("G_UK.txt");
		
        if (file_read.is_open()) {
            for (int i = 0; i < (nodes - vk) * vk && file_read >> string_read; i++) {
                g_uk[i] = stol(string_read,0,10);
            }
            file_read.close();
        }
        else {
            cout << "Failed to open G_UK.txt\n";
            return 1;
        }
    }	

//Read initial Vnodal//	

    file_read.open ("V_n.txt");
	
    if (file_read.is_open()) {
        for (int i = 0; i < nodes && file_read >> string_read; i++) {
            vnodal_initial[i] = stol(string_read,0,10);
        }
        file_read.close();
    }
    else {
        cout << "Failed to open V_n.txt\n";
        return 1;
    }

//Read initial Vbranch//

    file_read.open ("V_branch.txt");
	
    if (file_read.is_open()) {
        for (int i = 0; i < branches && file_read >> string_read; i++) {
            vbranch_initial[i] = stol(string_read,0,10);
        }
        file_read.close();
    }
    else {
        cout << "Failed to open V_branch.txt\n";
        return 1;
    }
	
//Read initial Ibranch//
	
    file_read.open ("I_br.txt");
	
    if (file_read.is_open()) {
        for (int i = 0; i < branches && file_read >> string_read; i++) {
            ibranch_initial[i] = stol(string_read,0,10);
        }
        file_read.close();
    }
    else {
        cout << "Failed to open I_br.txt\n";
        return 1;
    }

//Read input sources - needed for testbench and host application//

//Read voltage sources - if there are any//
		
    if (vk != 0) {

        file_read.open ("V_s.txt");
		
        if (file_read.is_open()) {
            for (int i = 0; i < samples * vk && file_read >> string_read; i++) {
                vs[i] = stol(string_read,0,10);
            }
            file_read.close();
        }
        else {
            cout << "Failed to open V_s.txt\n";
            return 1;
        }
    }
	
//Read current sources - if there are any//
	
    if (vk != src) {
		
        file_read.open ("I_s.txt");
		
        if (file_read.is_open()) {
            for (int i = 0; i < samples * (src - vk) && file_read >> string_read; i++) {
                is[i] = stol(string_read,0,10);
            }
            file_read.close();
        }
        else {
            cout << "Failed to open I_s.txt\n";
            return 1;
        }
    }
	
////////////////////////////////////////////////////////////////

//Calculation of number of multiplications - only for reference//
//helpful to decide which architecture should be used//
//but not really, since in most cases, number of multiplications is not equal to the number of DSPs used//	

    int counter_mul = 0;
	
    for (int i = 0; i < branches; i++) {
		
        if (coeff1_ibranch[i] != 0 ) {
            counter_mul++;
        }
        if (coeff2_ibranch[i] != 0 ) {
            counter_mul++;
        }
        if (coeff1_ihistory[i] != 0 ) {
            counter_mul++;
        }
        if (coeff2_ihistory[i] != 0 ) {
            counter_mul++;
        }
        if (coeff3_ihistory[i] != 0 ) {
            counter_mul++;
        }
    } 

	
    if (vk == 0) {
		
        for (int i = 0; i < nodes * nodes; i++) {
            if (g[i] != 0 ) {
                counter_mul++;
            }
        } 
    }
	
    else {
		
        for (int i = 0; i < (nodes - vk) * (nodes - vk); i++) {
            if (g_uu[i] != 0 ) {
                counter_mul++;
            }
        } 
	
        for (int i = 0; i < (nodes - vk) * vk; i++) {
            if (g_uk[i] != 0 ) {
                counter_mul++;
            }
        } 
    }
	
    std::cout << std::endl << std::endl << "total number of multiplications in the design = " << counter_mul << std::endl << std::endl << std::endl;

	
//End of Reading files//
////////////////////////////////////////////////////////////////

//Store all collected data in appropriate form in header files//
		
////////////////////////////////////////////////////////////////	
//Open file constants.h to store necessary arrays//
	
    file.open ("constants.h");
	
    if (!file.is_open()) {
        cout << "Failed to open constants.h\n";
        return 1;
    }
	
//Define Header File//

    file << "#ifndef _constants_h_\n";
    file << "#define _constants_h_\n\n";

//Define case of input sources//
 	
    if (vk == 0) {
        file << "#define current_sources\n\n";
    }
    else if (vk == src) {
        file << "#define voltage_sources\n\n";
    }
    else {
        file << "#define voltage_and_current_sources\n\n";
    }

//Define which achitecture will be used, depending on the number of nodes//
	
    if (nodes < limit_full_unroll) {

        file << "#define nodal_voltages_full_unroll\n\n";
    }
    else if (nodes < limit_partial_unroll) {

        file << "#define nodal_voltages_partial_unroll\n\n";
    }
    else {

        file << "#define nodal_voltages_pipelined\n\n";
    }

//Store coefficients//

    file << "const int f1 = " << f1 << ";\n";
    file << "const int f2 = " << f2 << ";\n";
    file << "const int nodes = " << nodes << ";\n";
    file << "const int branches = " << branches << ";\n";
	
    if (vk == 0) {
        file << "const int current_src = " << src << ";\n\n";
    }
    else if (vk == src) {
        file << "const int voltage_src = " << src << ";\n\n";
    }
    else {
        file << "const int voltage_src = " << vk << ";\n";
        file << "const int current_src = " << src - vk << ";\n\n";
    }
	
    file << "const int nodes_adj = " << nodes_adj << ";\n";
    file << "const int branches_adj = " << branches_adj << ";\n\n";
	
    if (nodes >= limit_full_unroll && nodes < limit_partial_unroll) {

        file << "const int unroll_factor = " << unroll_factor << ";\n";
        file << "const int nodes_even = " << nodes_even << ";\n\n";
    }
	

//Store coefficients for module history_currents//

    file << "const int Geff_voltage [branches] = {";
	
    for (int i = 0; i < branches; i++) {
        if (i != branches - 1) {
            file << coeff1_ihistory[i] << ",";
        }
        else {
            file << coeff1_ihistory[i] << "};\n\n";
        }
    }
	
    file << "const int Geff_current [branches] = {";
	
    for (int i = 0; i < branches; i++) {
        if (i != branches - 1) {
            file << coeff2_ihistory[i] << ",";
        }
        else {
            file << coeff2_ihistory[i] << "};\n\n";
        }
    }

    file << "const int Geff_transf [branches] = {";
	
    for (int i = 0; i < branches; i++) {
        if (i != branches - 1) {
            file << coeff3_ihistory[i] << ",";
        }
        else {
            file << coeff3_ihistory[i] << "};\n\n";
        }
    }


//Store coefficients for module branch_currents//	
	
    file << "const int Gbranch [branches] = {";
	
    for (int i = 0; i < branches; i++) {
        if (i != branches - 1) {
            file << coeff1_ibranch[i] << ",";
        }
        else {
            file << coeff1_ibranch[i] << "};\n\n";
        }
    }
	
    file << "const int Gbranch_transf [branches] = {";
	
    for (int i = 0; i < branches; i++) {
        if (i != branches - 1) {
            file << coeff2_ibranch[i] << ",";
        }
        else {
            file << coeff2_ibranch[i] << "};\n\n";
        }
    }


//Store coefficients for module nodal_currents//	

    file << "const int Transf [branches] = {";
	
    for (int i = 0; i < branches; i++) {
        if (i != branches - 1) {
            file << coeff_transf[i] << ",";
        }
        else {
            file << coeff_transf[i] << "};\n\n";
        }
    }


//Store coefficients common for more than one module//
	
	
//Store Branch Info - which nodes are connected to each branch//

    file << "const int BrnInfo_p [branches] = {";
	
    for (int i = 0; i < branches; i++) {
        if (i != branches - 1) {
            file << coeff1_branch_info[i] << ",";
        }
        else {
            file << coeff1_branch_info[i] << "};\n\n";
        }
    }
	
    file << "const int BrnInfo_n [branches] = {";
	
    for (int i = 0; i < branches; i++) {
        if (i != branches - 1) {
            file << coeff2_branch_info[i] << ",";
        }
        else {
            file << coeff2_branch_info[i] << "};\n\n";
        }
    }


//Store IsInfo - which nodes are connected to each current source - if there are any//	

    if (vk != src) {
				
        file << "const int IsInfo_p [current_src] = {";
		
        for (int i = 0; i < src - vk; i++) {
            if (i != src - vk - 1) {
                file << coeff1_is_info[i] << ",";
            }
            else {
                file << coeff1_is_info[i] << "};\n\n";
            }
        }
		
        file << "const int IsInfo_n [current_src] = {";
		
        for (int i = 0; i < src - vk; i++) {
            if (i != src - vk - 1) {
                file << coeff2_is_info[i] << ",";
            }
            else {
                file << coeff2_is_info[i] << "};\n\n";
            }
        }
    }
	

//Store VsInfo - which nodes are connected to each voltage source - if there are any//	
	
    if (vk != 0) {
	
        file << "const int VsInfo_p [voltage_src] = {";
		
        for (int i = 0; i < vk; i++) {
            if (i != vk - 1) {
                file << coeff1_vs_info[i] << ",";
            }
            else {
                file << coeff1_vs_info[i] << "};\n\n";
            }
        }
		
        file << "const int VsInfo_n [voltage_src] = {";
		
        for (int i = 0; i < vk; i++) {
            if (i != vk - 1) {
                file << coeff2_vs_info[i] << ",";
            }
            else {
                file << coeff2_vs_info[i] << "};\n\n";
            }
        }
    }
	
	
//if there are only current sources//		
//Store G - admittance matrix for module nodal_voltages//

    if (vk == 0) {
	
        file << "const int G [nodes][nodes] = {\n{";

        for (int i = 0; i < nodes; i++) {
            for (int j = 0; j < nodes; j++) {
                if (i == nodes - 1 && j == nodes - 1) {
                    file << g[i * nodes + j] << "}\n};\n\n";
                }
                else if (j == nodes - 1) {
                    file << g[i * nodes + j] << "},\n{";
                }
                else {
                    file << g[i * nodes + j] << ",";
                }				
            }	
        }
    }
	

//if there are voltage sources ,regardless of whether there are current sources//		
//Store G_UK - admittance matrix for module nodal_currents and G_UU - admittance matrix for module nodal_voltages//
	
    if (vk != 0) {
				
        file << "const int G_UK [nodes - voltage_src][voltage_src] = {\n{";

        for (int i = 0; i < nodes - vk; i++) {
            for (int j = vk; j > 0; j--) {
                if (i == nodes - vk - 1 && j == 1) {
                    file << g_uk[i * vk + j - 1] << "}\n};\n\n";
	        }
                else if (j == 1) { 
                    file << g_uk[i * vk + j - 1] << "},\n{";
                }
                else {
                    file << g_uk[i * vk + j - 1] << ",";
                }
            }
        }

        file << "const int G_UU [nodes - voltage_src][nodes - voltage_src] = {\n{";

        for (int i = 0; i < nodes - vk ; i++) {
            for (int j = 0; j < nodes - vk; j++) {
                if (i == nodes - vk - 1 && j == nodes - vk - 1) {
                    file << g_uu[i * (nodes - vk) + j] << "}\n};\n\n";
                }
                else if (j == nodes - vk - 1) {
                    file << g_uu[i * (nodes - vk) + j] << "},\n{";
                }
                else {
                    file << g_uu[i * (nodes - vk) + j] << ",";
                }	
            }
        }
    }		

	
//End of constants.h//

    file << "#endif\n\n";
	
    file.close();

//////////////////////////////////////////////////////////////////////////////



//Open file initial_state.h//

    file.open ("initial_state.h");
    if (!file.is_open()) {
        cout << "Failed to open initial_state.h\n";
        return 1;
    }
	
//Define Header File//

    file << "#ifndef _initial_state_h_\n";
    file << "#define _initial_state_h_\n\n";
	
//Store initial Vnodal//	

    file << "static int reg_Vnodal [nodes] = {";
	
    for (int i = 0; i < nodes; i++) {
        if (i != nodes - 1) {
            file << vnodal_initial[i] << ",";
        }
        else {
            file << vnodal_initial[i] << "};\n\n";
        }
    }
	
//Store initial Vbranch//

    file << "static int reg_Vbranch [branches] = {";
	
    for (int i = 0; i < branches; i++) {
        if (i != branches - 1){
            file << vbranch_initial[i] << ",";
        }
        else {
            file << vbranch_initial[i] << "};\n\n";
        }
    }
		
//Store initial Ibranch//

    file << "static int reg_Ibranch [branches] = {";
	
    for (int i = 0; i < branches; i++) {
        if (i != branches - 1) {
            file << ibranch_initial[i] << ",";
        }
        else {
            file << ibranch_initial[i] << "};\n\n";
        }
    }

	
//End of initial_state.h//	

    file << "#endif\n\n";
	
    file.close();
	
/////////////////////////////////////////////////////////////////////////////////



//Open file input_sources.h - needed for hls testbench and vitis host application//

    file.open ("input_sources.h");
    if (!file.is_open()) {
        cout << "Failed to open input_sources.h\n";
        return 1;
    }
	
//Define Header File//

    file << "#ifndef _input_sources_h_\n";
    file << "#define _input_sources_h_\n\n";
	
//Define number of samples//

    file << "#define samples " << samples + 1 << "\n\n";

//Store voltage sources - if there are any//

    if (vk != 0) {
	
//Adjust voltage sources array to correct number of samples //
	
        int vsource [samples + 1][vk];

//Reverse elements of array so as each row corresponds to each time sample//
		
        for (int i = 0; i < samples; i++) {
            for (int j = 0; j < vk; j++) {
                vsource[i][j] = vs[i + j * samples];
            }
        }			
        for (int j = 0; j < vk; j++) {	
            vsource[samples][j] = 0; // fill with zeros last sample
        }
				
//Store adjusted array of voltage sources//				
		
        file << "const int Vsource [samples][voltage_src] = {\n{";
		
        for (int i = 0; i < samples + 1; i++) {
            for (int j = 0; j < vk; j++) {
                if (i == samples && j == vk - 1) {
                    file << vsource[i][j] << "}\n};\n\n";
                }
                else if (j == vk - 1) {
                    file << vsource[i][j] << "},\n{";
                }
                else {
                    file << vsource[i][j] << ",";
                }
            }
        }
    }
	
//Store current sources - if there are any//
	
    if (vk != src) {
		
//Adjust current sources array to correct number of samples //	

        int isource [samples + 1][src - vk];

//Reverse elements of array so as each row corresponds to each time sample//
		
        for (int i = 0; i < samples; i++) {
            for (int j = 0; j < src - vk; j++) {
                isource[i][j] = is[i + j * samples];
            }
        }			
        for (int j = 0; j < src - vk; j++) {	
            isource[samples][j] = 0; // fill with zeros last sample
        }
				
//Store adjusted array of current sources//	
	
        file << "const int Isource [samples][current_src] = {\n{";
		
        for (int i = 0; i < samples + 1; i++) {
            for (int j = 0; j < src - vk; j++) {
                if (i == samples && j == src - vk - 1) {
                    file << isource[i][j] << "}\n};\n\n";
                }
                else if (j == src - vk - 1) {
                    file << isource[i][j] << "},\n{";
                }
                else {
                    file << isource[i][j] << ",";
                }
            }
        }
    }
	
//End of sources.h//

    file << "#endif\n\n";
	
    file.close();
	
////////////////////////////////////////////////////////////////	



//Open file ps_sim_host.h to store necessary information//
	
    file.open ("ps_sim_host.h");
	
    if (!file.is_open()) {
        cout << "Failed to open ps_sim_host.h\n";
        return 1;
    }
	
//Define Header File//

    file << "#ifndef _ps_sim_host_h_\n";
    file << "#define _ps_sim_host_h_\n\n";

//Define case of input sources//
 	
    if (vk == 0) {
        file << "#define current_sources\n\n";
    }
    else if (vk == src) {
        file << "#define voltage_sources\n\n";
    }
    else {
        file << "#define voltage_and_current_sources\n\n";
    }

///Define essential information//

    file << "#define f1 " << f1 << "\n";
    file << "#define f2 " << f2 << "\n";
    file << "#define nodes " << nodes << "\n";
    file << "#define branches " << branches << "\n";
	
    if (vk == 0) {
        file << "#define current_src " << src << "\n\n";
    }
    else if (vk == src) {
        file << "#define voltage_src " << src << "\n\n";
    }
    else {
        file << "#define voltage_src " << vk << "\n";
        file << "#define current_src " << src - vk << "\n\n";
    }
	    	
//End of ps_sim_host.h//

    file << "#endif\n\n";
	
    file.close();

//////////////////////////////////////////////////////////////////////////////
	
    return 0;
}

