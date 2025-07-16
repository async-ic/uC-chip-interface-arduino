/*
    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
    Copyright (C) 2025 Vincent Jassies - University of Groningen

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

#ifndef RECURRENCY_H
#define RECURRENCY_H

#include <cstdint>
#include <Arduino.h>

#include "core_ring_buffer.h"
#include "Interface_pin.h"
#include "datatypes.h"
#include "uc_boards.h"

#define MAX_NEURONS 80                        
#define MAX_CONNECTIONS_PER_NEURON 10        


struct OutputAddress {
  uint8_t output_neuron;
  uint8_t output_synapse;
};

struct RecurrencyNeuron {
  bool initialized;      // Flag to indicate if this slot has been set up.
  uint8_t input_neuron;  // This corresponds directly to the index.
  uint8_t count;         // Number of output addresses added.
  OutputAddress output_address[MAX_CONNECTIONS_PER_NEURON];
};

struct RecurrencyTable {
  RecurrencyNeuron recurrency_neurons[MAX_NEURONS];
};

// Declare the global variable
extern RecurrencyTable recurrency_table;

// Function declarations
void recurrency_check_and_forward(uint8_t neuron_address);
void process_pending_recurrent_events(void);
void buffer_neuron_event(uint8_t neuron_id);
void initRecurrencyNeuron(RecurrencyNeuron *neuron, uint8_t input);
bool addOutputAddress(RecurrencyNeuron *neuron, uint8_t out_neuron, uint8_t out_synapse);
bool addRecurrencyConnection(uint8_t input, uint8_t out_neuron, uint8_t out_synapse);

void send_output_addresses(const OutputAddress* addresses, int count);

#endif // RECURRENCY_H
