#include "power_system_simulator_test.h"

int main(void) {
	
    int Vnodal [samples][nodes];
    int Ibranch [samples][branches];

#ifdef voltage_sources

    int current_Vs [voltage_src];
    int current_Ib [branches_adj];	
    int current_Vn [nodes_adj];	

    for (int i = 0; i < samples; i++) {
		
        for (int j = 0; j < voltage_src; j++) {
            current_Vs[j] = Vsource[i][j];
        }
			
        power_system_simulator (current_Vs, current_Vn, current_Ib);
		
        for (int j = 0; j < nodes; j++) {
            Vnodal[i][j] = current_Vn[j];
        }
		
        for (int j = 0; j < branches; j++) {
            Ibranch[i][j] = current_Ib[j];
        }	
    }
	
#endif
	
#ifdef current_sources

    int current_Is [current_src];
    int current_Ib [branches_adj];	
    int current_Vn [nodes_adj];	
	
    for (int i = 0; i < samples; i++) {
		
        for (int j = 0; j < current_src; j++) {
            current_Is[j] = Isource[i][j];
        }
			
        power_system_simulator (current_Is, current_Vn, current_Ib);
		
        for (int j = 0; j < nodes; j++) {
            Vnodal[i][j] = current_Vn[j];
        }
		
        for (int j = 0; j < branches; j++) {
            Ibranch[i][j] = current_Ib[j];
        }	
    }
	
#endif

#ifdef voltage_and_current_sources

    int current_Vs [voltage_src];
    int current_Is [current_src];
    int current_Ib [branches_adj];	
    int current_Vn [nodes_adj];	
	
    for (int i = 0; i < samples; i++) {
		
        for (int j = 0; j < voltage_src; j++) {
            current_Vs[j] = Vsource[i][j];
        }
		
        for (int j = 0; j < current_src; j++) {
            current_Is[j] = Isource[i][j];
        }

        power_system_simulator (current_Vs, current_Is, current_Vn, current_Ib);
		
        for (int j = 0; j < nodes; j++) {
            Vnodal[i][j] = current_Vn[j];
        }
		
        for (int j = 0; j < branches; j++) {
            Ibranch[i][j] = current_Ib[j];
        }
    }
	
#endif	


    string Vnodal_directory = "HLS_V";
    string Ibranch_directory = "HLS_I";
	
#ifndef _WIN32

    if (mkdir(Vnodal_directory.c_str(), 0777) != 0 && errno != EEXIST) {
        cout << "Failed to create Vnodal directory: " << strerror(errno) << endl;
        return 1;
    }

    if (mkdir(Ibranch_directory.c_str(), 0777) != 0 && errno != EEXIST) {
        cout << "Failed to create Ibranch directory: " << strerror(errno) << endl;
        return 1;
    }

#else

    if (mkdir(Vnodal_directory.c_str()) != 0 && errno != EEXIST) {
        cout << "Failed to create Vnodal directory: " << strerror(errno) << endl;
        return 1;
    }

    if (mkdir(Ibranch_directory.c_str()) != 0 && errno != EEXIST) {
        cout << "Failed to create Ibranch directory: " << strerror(errno) << endl;
        return 1;
    }

#endif

    ofstream file_Vnodal;
    ofstream file_Ibranch;

    for (int j = 0; j < nodes; j++) {  
    
        string write_Vnodal = Vnodal_directory + "/V_" + to_string(j + 1) + ".txt";
        file_Vnodal.open(write_Vnodal);
	
        if (!file_Vnodal.is_open()) {
            cout << "Failed to open file V_ " << (j + 1) << endl;
            return 1;
        }

        for (int i = 0; i < samples; i++) {
            file_Vnodal << Vnodal[i][j] << endl;
        }

        file_Vnodal.close();
    }

    for (int j = 0; j < branches; j++) {  

        string write_Ibranch = Ibranch_directory + "/I_" + to_string(j + 1) + ".txt";
        file_Ibranch.open(write_Ibranch);
	
        if (!file_Ibranch.is_open()) {
            cout << "Failed to open file I_ " << (j + 1) << endl;
            return 1;
        }

        for (int i = 0; i < samples; i++) {
            file_Ibranch << Ibranch[i][j] << endl;
        }

        file_Ibranch.close();
    }
	
    return 0;
}


