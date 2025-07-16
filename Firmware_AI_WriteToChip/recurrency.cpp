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

#include "recurrency.h"

// Define the global recurrency_table
RecurrencyTable recurrency_table;

// The offset is needed because the instructions are being sent in the range of 101 to 180 (which indicate neuron 1 to 80) 
#define INPUT_NEURON_OFFSET   100
#define OUTPUT_NEURON_OFFSET  100


void initRecurrencyNeuron(RecurrencyNeuron *neuron, uint8_t input) {
  neuron->input_neuron = input;
  neuron->count = 0;
  neuron->initialized = true;
}

bool addOutputAddress(RecurrencyNeuron *neuron, uint8_t out_neuron, uint8_t out_synapse) {
  if (neuron->count < MAX_CONNECTIONS_PER_NEURON) {
    neuron->output_address[neuron->count].output_neuron = out_neuron;
    neuron->output_address[neuron->count].output_synapse = out_synapse;
    neuron->count++;
    return true;
  }
  return false;
}

bool addRecurrencyConnection(uint8_t input, uint8_t out_neuron, uint8_t out_synapse) {
  bool saved_succesfully = false;

  input -= INPUT_NEURON_OFFSET;
  RecurrencyNeuron *neuron = &recurrency_table.recurrency_neurons[input];

  // Initialize the neuron if it hasn't been set up yet.
  if (!neuron->initialized) {
    initRecurrencyNeuron(neuron, input);
  }
  
  out_neuron -= OUTPUT_NEURON_OFFSET;
  // Add the new output connection.
  saved_succesfully = addOutputAddress(neuron, out_neuron, out_synapse);

  // send some kind of status return
  error_message_bypass_buffer(OUT_ERROR, neuron, out_neuron, out_synapse);

  //send_package("saved connection: %i %i %i\n", neuron->input_neuron, neuron->output_address[neuron->count].output_neuron, neuron->output_address[neuron->count].output_synapse);
  return 1;
}
