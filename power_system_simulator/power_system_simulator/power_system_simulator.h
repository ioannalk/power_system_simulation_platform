#ifndef _power_system_simulator_h_
#define _power_system_simulator_h_

#include "modules.h"


#ifdef voltage_sources

void power_system_simulator (int (&) [voltage_src], int (&) [nodes_adj], int (&) [branches_adj]);

#endif


#ifdef current_sources

void power_system_simulator (int (&) [current_src], int (&) [nodes_adj], int (&) [branches_adj]);

#endif


#ifdef voltage_and_current_sources

void power_system_simulator (int (&) [voltage_src], int (&) [current_src], int (&) [nodes_adj], int (&) [branches_adj]);

#endif

#endif
