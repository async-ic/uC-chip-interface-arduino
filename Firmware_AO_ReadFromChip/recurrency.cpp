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
#include "ethernet_sender.h"  // put this in .cpp only


// Define the global recurrency_table
RecurrencyTable recurrency_table;

// The offset is needed because the instructions are being sent in the range of 101 to 180 (which indicate neuron 1 to 80) 
#define INPUT_NEURON_OFFSET   100
#define OUTPUT_NEURON_OFFSET  100

#define MAX_PENDING_NEURON_EVENTS 64

static uint8_t pending_neuron_events[MAX_PENDING_NEURON_EVENTS];
static volatile uint8_t pending_start = 0;
static volatile uint8_t pending_end = 0;

// This function takes a spiking neuron's address and checks the recurrency table.
// If entries are found, it sends them using the ethernet sender.
void recurrency_check_and_forward(uint8_t neuron_address) {
    if (neuron_address >= MAX_NEURONS) return;

    RecurrencyNeuron* rec = &recurrency_table.recurrency_neurons[neuron_address];

    if (!rec->initialized || rec->count == 0) return;

    Serial.printf("[Recurrency] Neuron %u spiked with %u connections\n", neuron_address, rec->count);

    // Send all OutputAddresses in one message (bulk send)
    send_output_addresses(rec->output_address, rec->count);
}

void process_pending_recurrent_events() {
  while (pending_start != pending_end) {
    uint8_t input_neuron = pending_neuron_events[pending_start];
    pending_start = (pending_start + 1) % MAX_PENDING_NEURON_EVENTS;

    RecurrencyNeuron& neuron = recurrency_table.recurrency_neurons[input_neuron];

    if (neuron.initialized && neuron.count > 0) {
      ethernet_sender_send((const byte*)neuron.output_address,
                           neuron.count * sizeof(OutputAddress));
    }
  }
}

void buffer_neuron_event(uint8_t neuron_id) {
  uint8_t next = (pending_end + 1) % MAX_PENDING_NEURON_EVENTS;
  if (next != pending_start) {  // not full
    pending_neuron_events[pending_end] = neuron_id;
    pending_end = next;
  }
}

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
  error_message_bypass_buffer(OUT_ERROR, input, out_neuron, out_synapse);

  //send_package("saved connection: %i %i %i\n", neuron->input_neuron, neuron->output_address[neuron->count].output_neuron, neuron->output_address[neuron->count].output_synapse);
  return saved_succesfully;
}
