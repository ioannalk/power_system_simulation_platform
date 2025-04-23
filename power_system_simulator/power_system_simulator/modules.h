#ifndef _modules_h_
#define _modules_h_

#include <stdint.h>
#include "constants.h"

void history_currents (int (&) [branches], int (&) [branches], int (&) [branches]);

void branch_voltages (int (&) [nodes], int (&) [branches]);

void branch_currents (int (&) [branches], int (&) [branches], int (&) [branches]);

void store_Vnodal (int (&) [nodes], int (&) [nodes_adj]);

void store_Ibranch (int (&) [branches], int (&) [branches_adj]);


#ifdef voltage_sources

void load_Vsource (int (&) [voltage_src], int (&) [voltage_src]);

void nodal_currents (int (&) [voltage_src], int (&) [branches], int (&) [nodes - voltage_src]);

void nodal_voltages (int (&) [voltage_src], int (&) [nodes - voltage_src], int (&) [nodes]);

#endif


#ifdef current_sources

void load_Isource (int (&) [current_src], int (&) [current_src]);

void nodal_currents (int (&) [current_src], int (&) [branches], int (&) [nodes]);

void nodal_voltages (int (&) [nodes], int (&) [nodes]);

#endif


#ifdef voltage_and_current_sources

void load_Vsource (int (&) [voltage_src], int (&) [voltage_src]);

void load_Isource (int (&) [current_src], int (&) [current_src]);

void nodal_currents (int (&) [voltage_src], int (&) [current_src], int (&) [branches], int (&) [nodes - voltage_src]);

void nodal_voltages (int (&) [voltage_src], int (&) [nodes - voltage_src], int (&) [nodes]);

#endif


#endif
