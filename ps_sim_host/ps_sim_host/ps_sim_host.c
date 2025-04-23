#include <stdio.h>
#include <stdlib.h>
#include "xil_printf.h"
#include "platform.h"
#include "xil_cache.h"
#include "ff.h"
#include "xpower_system_simulator.h"
#include "ps_sim_host.h"
#include "input_sources.h"

// Read HLS text files and compare results 
#define read_sw_results

// Write FPGA results to text files 
#define write_hw_results

//Declare arrays for data storage
int Vnodal[samples][nodes] __attribute__((section(".ddr")));
int Ibranch[samples][branches] __attribute__((section(".ddr")));

int main() {
	
    // Initialize platform
    init_platform();
	
    // Create instance and configure HLS IP
    XPower_system_simulator ps_sim;
    XPower_system_simulator_Config *ps_sim_cfg;

    // Lookup configuration for HLS IP
    ps_sim_cfg = XPower_system_simulator_LookupConfig(XPAR_XPOWER_SYSTEM_SIMULATOR_0_DEVICE_ID);
    if (!ps_sim_cfg) {
        xil_printf("ERROR: HLS IP LookupConfig failed!\r\n");
        return XST_FAILURE;
    }
	
    // Initialize HLS IP with its configuration
    int status = XPower_system_simulator_CfgInitialize(&ps_sim, ps_sim_cfg);
    if (status != XST_SUCCESS) {
        xil_printf("ERROR: HLS IP CfgInitialize failed!\r\n");
        return status;
    }
	
    xil_printf("HLS IP Initialized!\r\n");
	
    // Verify that the HLS IP is ready
    if (!XPower_system_simulator_IsReady(&ps_sim)) {
        xil_printf("ERROR: HLS IP not ready!\r\n");
        return XST_FAILURE;
    }
	
    xil_printf("HLS IP Ready!\r\n");
    
    // Declare arrays for current simulation sample
    __attribute__((aligned(64))) int current_Vnodal [nodes];
    __attribute__((aligned(64))) int current_Ibranch [branches];

#ifdef voltage_sources
	
    // Declare array for current source sample
    int current_Vsource [voltage_src];

    // Set address for HLS IP to use the Vsource data
    XPower_system_simulator_Set_Vsource(&ps_sim, (u64)current_Vsource);
    u64 addr_Vsource = XPower_system_simulator_Get_Vsource(&ps_sim);
    if (addr_Vsource != (u64)current_Vsource) {
        xil_printf("ERROR: Vsource Set/Get failed!\r\n");
    }
	
#endif

#ifdef current_sources

    // Declare array for current source sample
    int current_Isource [current_src];

    // Set address for HLS IP to use the Isource data
    XPower_system_simulator_Set_Isource(&ps_sim, (u64)current_Isource);
    u64 addr_Isource = XPower_system_simulator_Get_Isource(&ps_sim);
    if (addr_Isource != (u64)current_Isource) {
        xil_printf("ERROR: Isource Set/Get failed!\r\n");
    }
	
#endif

#ifdef voltage_and_current_sources
   
    // Declare arrays for current source sample
    int current_Vsource [voltage_src];
    int current_Isource [current_src];

    // Set address for HLS IP to use the Vsource data
    XPower_system_simulator_Set_Vsource(&ps_sim, (u64)current_Vsource);
    u64 addr_Vsource = XPower_system_simulator_Get_Vsource(&ps_sim);
    if (addr_Vsource != (u64)current_Vsource) {
        xil_printf("ERROR: Vsource Set/Get failed!\r\n");
    }
	
    // Set address for HLS IP to use the Isource data
    XPower_system_simulator_Set_Isource(&ps_sim, (u64)current_Isource);
    u64 addr_Isource = XPower_system_simulator_Get_Isource(&ps_sim);
    if (addr_Isource != (u64)current_Isource) {
        xil_printf("ERROR: Isource Set/Get failed!\r\n");
    }
	
#endif
		
    // Set address for HLS IP to store the Vnodal data
    XPower_system_simulator_Set_Vnodal(&ps_sim, (u64)current_Vnodal);
    u64 addr_Vnodal = XPower_system_simulator_Get_Vnodal(&ps_sim);
    if (addr_Vnodal != (u64)current_Vnodal) {
        xil_printf("ERROR: Vnodal Set/Get failed!\r\n");
    }
		
    // Set address for HLS IP to store the Ibranch data
    XPower_system_simulator_Set_Ibranch(&ps_sim, (u64)current_Ibranch);
    u64 addr_Ibranch = XPower_system_simulator_Get_Ibranch(&ps_sim);
    if (addr_Ibranch != (u64)current_Ibranch) {
        xil_printf("ERROR: Ibranch Set/Get failed!r\n");
    }
	
    xil_printf("HLS IP Set!\r\n");	

    // Declare variables for measuring latency, II, and total execution time
    u64 start, stop, start_ip, stop_ip;
    u64 total_ip_cycles = 0;
    u64 total_ii_cycles = 0;
    u64 previous_start_ip = 0;
    u64 total_cycles = 0;
	
    // Capture start time
    asm volatile("mrs %0, cntvct_el0" : "=r"(start));

    for (int i = 0; i < samples; i++) {

#ifdef voltage_sources 
	
        // Copy Vsource data for the current sample
        for (int j = 0; j < voltage_src; j++) {
            current_Vsource[j] = Vsource[i][j];
        }

        // We want the processor to write to DDR not ARM cached data
        Xil_DCacheFlushRange((INTPTR)current_Vsource, sizeof(current_Vsource));
		
#endif	

#ifdef current_sources

        // Copy Isource data for the current sample
        for (int j = 0; j < current_src; j++) {
            current_Isource[j] = Isource[i][j];
        }

        // We want the processor to write to DDR not ARM cached data
        Xil_DCacheFlushRange((INTPTR)current_Isource, sizeof(current_Isource));
		
#endif

#ifdef voltage_and_current_sources

        // Copy Vsource data for the current sample
        for (int j = 0; j < voltage_src; j++) {
            current_Vsource[j] = Vsource[i][j];
        }

        // We want the processor to write to DDR not ARM cached data
        Xil_DCacheFlushRange((INTPTR)current_Vsource, sizeof(current_Vsource));
		
        // Copy Isource data for the current sample
        for (int j = 0; j < current_src; j++) {
            current_Isource[j] = Isource[i][j];
        }

        // We want the processor to write to DDR not ARM cached data
        Xil_DCacheFlushRange((INTPTR)current_Isource, sizeof(current_Isource));
		
#endif

        // Capture start time of the IP
        asm volatile("mrs %0, cntvct_el0" : "=r"(start_ip));

        // Start the IP
        XPower_system_simulator_Start(&ps_sim);
		
        // Wait for the IP to complete	
        while (!XPower_system_simulator_IsDone(&ps_sim));
		
        // Capture end time of the IP
        asm volatile("mrs %0, cntvct_el0" : "=r"(stop_ip));

        // Calculate IP latency
        total_ip_cycles += (stop_ip - start_ip);
		
        // Calculate II
        if (i > 0) {
            total_ii_cycles += (start_ip - previous_start_ip);
        }
        previous_start_ip = start_ip;

        // HLS IP wrote to DDR, so we want the processor to read from DDR not ARM cached data
        Xil_DCacheInvalidateRange((INTPTR)current_Vnodal, sizeof(current_Vnodal));

        // Store results for Vnodal
        for (int j = 0; j < nodes; j++) {
            Vnodal[i][j] = current_Vnodal[j];
        }

        // HLS IP wrote to DDR, so we want the processor to read from DDR not ARM cached data
        Xil_DCacheInvalidateRange((INTPTR)current_Ibranch, sizeof(current_Ibranch));

        // Store results for Ibranch
        for (int j = 0; j < branches; j++) {
            Ibranch[i][j] = current_Ibranch[j];
        }
    }

    // Capture stop time
    asm volatile("mrs %0, cntvct_el0" : "=r"(stop));
    total_cycles = (stop - start);

    xil_printf("HLS IP done!\r\n");

    // Print IP latency, II, and total execution
    xil_printf("Total Execution Time: %lu cycles\r\n", total_cycles);
    xil_printf("Total Initiation Interval: %lu cycles\r\n", total_ii_cycles);
    xil_printf("Total IP Latency: %lu cycles\r\n", total_ip_cycles);
    xil_printf("Average Initiation Interval: %lu cycles\r\n", total_ii_cycles / (samples - 1));
    xil_printf("Average IP Latency: %lu cycles\r\n", total_ip_cycles / samples);

    // Create object for SD Card
    FATFS fatfs; 
    FRESULT fresult; 
	
    // Mount SD card
    fresult = f_mount(&fatfs, "0:", 0); 
    if (fresult) {
        xil_printf("ERROR %d: Mounting SD card failed!\r\n", fresult);
        return XST_FAILURE;
    }

#ifdef read_sw_results
	
    // Compare results with HLS results

    // Read HLS Vnodal files
    for (int j = 0; j < nodes; j++) {  
	
        // Create filename for HLS Vnodal file
        char name_Vnodal[20];
        snprintf(name_Vnodal, sizeof(name_Vnodal), "HLS_V/V_%d.txt", (j + 1));
        
        // Open HLS Vnodal file 
        FIL file_Vnodal;
        fresult = f_open(&file_Vnodal, name_Vnodal, FA_READ);
        if (fresult) {
            xil_printf("ERROR %d: Opening %s failed!\r\n", fresult, name_Vnodal);
            return XST_FAILURE;
        }

        // Set file pointer to the beginning of HLS file
        fresult = f_lseek(&file_Vnodal, 0);
        if(fresult) {
            xil_printf("ERROR %d: Setting %s failed!\r\n", fresult, name_Vnodal);
            return XST_FAILURE;
        }
		
        // Read HLS Vnodal file and compare data
        for (int i = 0; i < samples; i++) {
            char tmp[20];
            char tmp_ch;
            UINT b_read;
            int k = 0;

            while (k < sizeof(tmp) - 1) {
                fresult = f_read(&file_Vnodal, &tmp_ch, 1, &b_read);
                if (fresult || b_read == 0) {
                    xil_printf("ERROR %d: Reading %s failed!\r\n", fresult, name_Vnodal);
                    return XST_FAILURE;
                }
                if (tmp_ch == '\n' || tmp_ch == '\r') break;
                tmp[k++] = tmp_ch;
            }
            tmp[k] = '\0';
            int hls_tmp = atoi(tmp);
            if (hls_tmp != Vnodal[i][j]) {
                xil_printf("ERROR : Mismatch in Vnodal[%d][%d] : ", i, j);
                xil_printf("HLS value: %d, FPGA value: %d!\r\n", hls_tmp, Vnodal[i][j]);
                return XST_FAILURE;
            }
        }
		
        // Close HLS Vnodal file
        fresult = f_close(&file_Vnodal);
        if (fresult) {
            xil_printf("ERROR %d: Closing %s failed!\r\n", fresult, name_Vnodal);
            return XST_FAILURE;
        }
    }

    xil_printf("HLS Vnodal data matches computed results!\r\n");

    // Read HLS Ibranch files
    for (int j = 0; j < branches; j++) {  
	
        // Create filename for Ibranch file
        char name_Ibranch[20];
        snprintf(name_Ibranch, sizeof(name_Ibranch), "HLS_I/I_%d.txt", (j + 1));
        
        // Open HLS Ibranch file 
        FIL file_Ibranch;
        fresult = f_open(&file_Ibranch, name_Ibranch, FA_READ);
        if (fresult) {
            xil_printf("ERROR %d: Opening %s failed!\r\n", fresult, name_Ibranch);
            return XST_FAILURE;
        }

        // Set file pointer to the beginning of HLS file
        fresult = f_lseek(&file_Ibranch, 0);
        if(fresult) {
            xil_printf("ERROR %d: Setting %s failed!\r\n", fresult, name_Ibranch);
            return XST_FAILURE;
        }
		
        // Read HLS Ibranch file and compare data
        for (int i = 0; i < samples; i++) {
            char tmp[20];
            char tmp_ch;
            UINT b_read;
            int k = 0;

            while (k < sizeof(tmp) - 1) {
                fresult = f_read(&file_Ibranch, &tmp_ch, 1, &b_read);
                if (fresult || b_read == 0) {
                    xil_printf("ERROR %d: Reading %s failed!\r\n", fresult, name_Ibranch);
                    return XST_FAILURE;
                }
                if (tmp_ch == '\n' || tmp_ch == '\r') break;
                tmp[k++] = tmp_ch;
            }
            tmp[k] = '\0';
            int hls_tmp = atoi(tmp);
            if (hls_tmp != Ibranch[i][j]) {
                xil_printf("ERROR : Mismatch in Ibranch[%d][%d] : ", i, j);
                xil_printf("HLS value: %d, FPGA value: %d!\r\n", hls_tmp, Ibranch[i][j]);
                return XST_FAILURE;
            }
        }
		
        // Close Ibranch file
        fresult = f_close(&file_Ibranch);
        if (fresult) {
            xil_printf("ERROR %d: Closing %s failed!\r\n", fresult, name_Ibranch);
            return XST_FAILURE;
        }
    }
	
    xil_printf("HLS Ibranch data matches computed results!\r\n");
	
#endif

#ifdef write_hw_results
	
    // Copy results to SD Card

    // Create directory for Vnodal
    fresult = f_mkdir("FPGA_V");
    if (fresult) {
        xil_printf("ERROR %d: Creating directory FPGA_V failed!\r\n", fresult);
        return XST_FAILURE; 
    }
		
    // Write Vnodal data to files
    for (int j = 0; j < nodes; j++) {  
	
        // Create filename for Vnodal file
        char name_Vnodal[20];
        snprintf(name_Vnodal, sizeof(name_Vnodal), "FPGA_V/V_%d.txt", (j + 1));
        
        // Open Vnodal file 
        FIL file_Vnodal;
        fresult = f_open(&file_Vnodal, name_Vnodal, FA_CREATE_ALWAYS | FA_WRITE);
        if (fresult) {
            xil_printf("ERROR %d: Opening %s failed!\r\n", fresult, name_Vnodal);
            return XST_FAILURE;
        }

        // Set file pointer to the beginning of file
        fresult = f_lseek(&file_Vnodal, 0);
        if(fresult) {
            xil_printf("ERROR %d: Setting %s failed!\r\n", fresult, name_Vnodal);
            return XST_FAILURE;
        }
		
        // Write to Vnodal file
        for (int i = 0; i < samples; i++) {
            char tmp[20];
            snprintf(tmp, sizeof(tmp), "%d\n", Vnodal[i][j]);
            fresult = f_write(&file_Vnodal, tmp, strlen(tmp), NULL);
            if (fresult) {
                xil_printf("ERROR %d: Writing data to %s failed!\r\n", fresult, name_Vnodal);
                return XST_FAILURE;
            }
        }
		
        // Sync Vnodal data to SD card
        fresult = f_sync(&file_Vnodal);
        if (fresult) {
            xil_printf("ERROR %d: Syncing %s failed!\r\n", fresult, name_Vnodal);
            return XST_FAILURE;
        }
		
        // Close Vnodal file
        fresult = f_close(&file_Vnodal);
        if (fresult) {
            xil_printf("ERROR %d: Closing %s failed!\r\n", fresult, name_Vnodal);
            return XST_FAILURE;
        }
    }

    xil_printf("Vnodal data successfully written to SD card!\r\n");

    // Create directory for Ibranch
    fresult = f_mkdir("FPGA_I");
    if (fresult) {
        xil_printf("ERROR %d: Creating directory FPGA_I failed!\r\n", fresult);
        return XST_FAILURE; 
    }
	
    // Write Ibranch data to files
    for (int j = 0; j < branches; j++) {  
	
        // Create filename for Ibranch file
        char name_Ibranch[20];
        snprintf(name_Ibranch, sizeof(name_Ibranch), "FPGA_I/I_%d.txt", (j + 1));
        
        // Open Ibranch file 
        FIL file_Ibranch;
        fresult = f_open(&file_Ibranch, name_Ibranch, FA_CREATE_ALWAYS | FA_WRITE);
        if (fresult) {
            xil_printf("ERROR %d: Opening %s failed!\r\n", fresult, name_Ibranch);
            return XST_FAILURE;
        }

        // Set file pointer to the beginning of file
        fresult = f_lseek(&file_Ibranch, 0);
        if(fresult) {
            xil_printf("ERROR %d: Setting %s failed!\r\n", fresult, name_Ibranch);
            return XST_FAILURE;
        }
		
        // Write to Ibranch file
        for (int i = 0; i < samples; i++) {
            char tmp[20];
            snprintf(tmp, sizeof(tmp), "%d\n", Ibranch[i][j]);
            fresult = f_write(&file_Ibranch, tmp, strlen(tmp), NULL);
            if (fresult) {
                xil_printf("ERROR %d: Writing data to %s failed!\r\n", fresult, name_Ibranch);
                return XST_FAILURE;
            }
        }

        // Sync Ibranch data to SD card
        fresult = f_sync(&file_Ibranch);
        if (fresult) {
            xil_printf("ERROR %d: Syncing %s failed!\r\n", fresult, name_Ibranch);
            return XST_FAILURE;
        }
		
        // Close Ibranch file
        fresult = f_close(&file_Ibranch);
        if (fresult) {
            xil_printf("ERROR %d: Closing %s failed!\r\n", fresult, name_Ibranch);
            return XST_FAILURE;
        }
    }
	
    xil_printf("Ibranch data successfully written to SD card!\r\n");

#endif

    // Unmount SD card
    fresult = f_mount(NULL, "0:", 0);
    if (fresult) {
        xil_printf("ERROR %d: Unmounting SD card failed!\r\n", fresult);
        return XST_FAILURE;
    }
	
    // Clean up platform
    cleanup_platform();
	
    xil_printf("Exiting Application!\r\n");

    return 0;
}
