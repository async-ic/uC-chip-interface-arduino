/*
    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
    Copyright (C) 2022-2023 Ole Richter - University of Groningen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef INSTRUCTION_EXEC_H
#define INSTRUCTION_EXEC_H

//EXEC_PRESISION in usec
#define EXEC_PRESISION  100 

#include "core_ring_buffer.h"
#include "datatypes.h"
#include "interface_AER_from_chip.h"
#include "interface_AER_to_chip.h"
#include "interface_spi.h"
#include "interface_pin.h"
#include "interface_i2c.h"
#include "core_intervaltimer_samd21.h"
#include "recurrency.h"

/*
the instruction execution handles the 
*/

extern volatile bool exec_active;
extern volatile IntervalTimer myTimer;

void set_time_offset(uint32_t offset);
void read_time(bool is_ISR_call);
void exec_instruction(packet_t* instruction, bool is_ISR_call);
void exec_timed_commands();

#endif