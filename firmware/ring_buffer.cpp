/*
    This file is part of the Firmware project to interface with small Neuromorphic chips
    Copyright (C) 2022 Ole Richter - University of Groningen

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

#include "ring_buffer.h"

volatile packet_t input_ring_buffer[INPUT_BUFFER_SIZE] = {};
volatile uint16_t input_ring_buffer_start = 0;
volatile uint16_t input_ring_buffer_next_free = 0;
volatile packet_t output_ring_buffer[OUTPUT_BUFFER_SIZE] = {};
volatile uint16_t output_ring_buffer_start = 0;
volatile uint16_t output_ring_buffer_next_free = 0;
volatile bool output_buffer_overflow = 0;

volatile unsigned long offset_time = 0; // offset time is in microseconds, 0 also doubles as no recording
volatile bool read_active = 0;

void setup_ring_buffer(){
  //input_ring_buffer = (packet_t*) malloc(sizeof(packet_t)*INPUT_BUFFER_SIZE);
  //output_ring_buffer = (packet_t*) malloc(sizeof(packet_t)*OUTPUT_BUFFER_SIZE);
  output_ring_buffer_start = 0;
  output_ring_buffer[output_ring_buffer_start].data.header = OUT_BUFFER_LAST_READ;
  output_ring_buffer[output_ring_buffer_start].data.value = 1;
  output_ring_buffer[output_ring_buffer_start].data.exec_time = micros()-offset_time;
  output_ring_buffer_next_free =  (output_ring_buffer_start +1) % OUTPUT_BUFFER_SIZE;
  input_ring_buffer_next_free = 0;
  input_ring_buffer_start = 0;

}


bool is_output_buffer_not_full(){
  if (output_ring_buffer_start == (output_ring_buffer_next_free + 1) % OUTPUT_BUFFER_SIZE) {
    output_ring_buffer[output_ring_buffer_next_free].error.header = OUT_ERROR_OUTPUT_FULL;
    output_ring_buffer[output_ring_buffer_next_free].error.org_header = OUT_ERROR_OUTPUT_FULL;
    output_ring_buffer[output_ring_buffer_next_free].error.value = 1;
    output_ring_buffer_next_free = (output_ring_buffer_next_free + 1) % OUTPUT_BUFFER_SIZE; 
    return false;         
  } 
  else if(output_ring_buffer_start == output_ring_buffer_next_free) {
    uint16_t index = output_ring_buffer_next_free == 0 ? OUTPUT_BUFFER_SIZE-1 : output_ring_buffer_next_free-1;
    output_ring_buffer[index].data.value++;
    return false;
  }
  else return true;
}

void send_output_ring_buffer(){

  noInterrupts(); 
  if (!read_active){

    read_active = true;
    interrupts(); 
    while (output_ring_buffer_start != output_ring_buffer_next_free){
      noInterrupts();
      packet_t out = copy_packet(&output_ring_buffer[output_ring_buffer_start]);
      output_ring_buffer_start = (output_ring_buffer_start + 1) % OUTPUT_BUFFER_SIZE; 
      interrupts();
          // due to different storage alignment need to write data bytes individual
          uint8_t position = 0;
    for (position = 0; position < sizeof(packet_t); position++) {
      Serial.write(out.bytes[position]);
    }
      //Serial.write(out.bytes,sizeof(packet_t));
      
    }
    noInterrupts(); 
    output_ring_buffer[output_ring_buffer_next_free].data.header = OUT_BUFFER_LAST_READ;
    output_ring_buffer[output_ring_buffer_next_free].data.value = 1;
    output_ring_buffer[output_ring_buffer_next_free].data.exec_time = micros()-offset_time;
    output_ring_buffer_next_free = (output_ring_buffer_next_free + 1) % OUTPUT_BUFFER_SIZE; 
    read_active = false;
    interrupts();
  }

}

void send_output_ring_buffer_last(){

  noInterrupts(); 
  if (!read_active){

    read_active = true;
    interrupts(); 
    if (output_ring_buffer_next_free-1 >= 0){
      noInterrupts();
      packet_t out = copy_packet(&output_ring_buffer[output_ring_buffer_next_free-1]);
      interrupts();
          // due to different storage alignment need to write data bytes individual
           uint8_t position = 0;
    for (position = 0; position < sizeof(packet_t); position++) {
      Serial.write(out.bytes[position]);
    }
      //Serial.write(out.bytes, sizeof(packet_t));
      
    }
    read_active = false;
  }

}

void send_input_ring_buffer(){

  digitalWrite(3, HIGH);
  
  uint32_t ring_buffer_start = input_ring_buffer_start;
  interrupts(); 
  while (ring_buffer_start != input_ring_buffer_next_free){

    noInterrupts();
    packet_t out = copy_packet(&input_ring_buffer[ring_buffer_start]);
    ring_buffer_start = (ring_buffer_start + 1) % INPUT_BUFFER_SIZE; 
    interrupts();
    //Serial.write(out.bytes,sizeof(packet_t));
        // due to different storage alignment need to write data bytes individual
         uint8_t position = 0;
    for (position = 0; position < sizeof(packet_t); position++) {
      Serial.write(out.bytes[position]);
    }
  }

}


