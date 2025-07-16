/*
    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
    Copyright (C) 2022-2023 Ole Richter - University of Groningen
    Copyright (C) 2022 Benjamin Hucko
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

    24/03/25 - The firmware has been split in two, one for the AI (writing to the chip) and one for AO (reading from chip) 
    Latest update creates the possibility to have recurrency, sending a recurrency table to the AO microcontroller with the recurrency connections.
    Then sending data to the chip, reading the return, checking the recurrency table and forwarding the data over an Ethernet connection between the two controllers.  
*/

#include <Arduino.h>
#include "core_ring_buffer.h"
#include "core_intervaltimer_samd21.h"
#include "core_instruction_exec.h"

/*
 This programm is supposed to be used for interfacing with small Async or Neuromorphic chips,
 the pin and interface configuration is uploaded on runtime via configuration packets,
 so for most chips/modules a change of the uC firmware is not nessesary.

 the Idea behind the firmware is that you can send command packets with a exec time in the future, 
and the command is executed at that time via interrupt. (see instruction_exec.h/cpp)
 if you send exec_time=0 the execution is done right away. The exec_time needs to be sorted and assending. 
 the execuion is done via interrupts the execution only happens every ~100us => see instruction_exec.h
 timesteps are in usec and the role over occurs after ~71min after boot of the uC. roleover is not supported.

 the folloeing interfaces are supported:
 - SPI - Serial Periferal Interface for memory and fifo access (see "Interface_spi.h/cpp")
 - AER - paralell databus with 4-phase handshake (see "AER_tochip.h/cpp" and "AER_from_chip.h/cpp")
 - PIN - manual pin raising and lowering (see "Interface_pin.h/cpp")
 

 see datatypes.h for all available commands and thier use.

 to start recording and execution set the TIME to anything other than 0, to stop execution and recording sent TIME to 0.

 recording of events is handeled in "isr_helper.h/cpp"

 the coding style is following the Arduino Style of having interfaces staticly allocatated for access. 
 In a future version this shoud be at least changed to static Object Oriented Programing

*/

#include "ethernet_sender.h"
#include "recurrency.h"

byte mac[] = { 0x04, 0xE9, 0xE5, 0x10, 0x00, 0x01 };
IPAddress ip(192, 168, 1, 100);
IPAddress serverIP(192, 168, 1, 101);
const int port = 8888;

uint8_t dataToSend[1024];

/*
 setup starts the serial connection and allocates the ring buffers
 aswell as sets the interruptpriority for command execution
*/
void setup() { 
  Serial.begin(115200);
  Serial.setTimeout(1);    
  setup_ring_buffer();
  
  ((IntervalTimer*)&myTimer)->priority(200);

  ethernet_sender_init(mac, ip, serverIP, port);
}


static unsigned long lastTrigger = 0;
/*
 the main loop just handels comunication with the host via the serial interface and instuction execution if exec_time == 0, 
 in all other cases the instruction is stored in the instruction ring buffer.
*/
void loop() {
  // // simulate trigger every 5 seconds
  // if (millis() - lastTrigger > 5000) {
  //     lastTrigger = millis();
      
  //     int numToSend = 3;
  //     send_output_addresses(numToSend);
  // }
  process_pending_recurrent_events();

  if (Serial.available() >= (int)sizeof(packet_t)) {
    // read instruction packet
    packet_t current_instruction; 

    // because we use different storage allignment we need to transfer each byte individual
    // also checks if the communication protocol needs to be aligned
    uint8_t position;
    for (position = 0; position < sizeof(packet_t); position++){
      Serial.readBytes((char*)&current_instruction.bytes[position], 1);
      // check if the PC requests a communication protocol alignment by writing 9 bytes 
      // of IN_ALIGN_COMMUNICATION_PROTOCOL (255) so that uC catches one of them as a header
      if (position == 0 && current_instruction.bytes[0] == IN_ALIGN_COMMUNICATION_PROTOCOL){
        do {
          Serial.readBytes((char*)&current_instruction.bytes[0], 1);
        } while (current_instruction.bytes[0] == IN_ALIGN_COMMUNICATION_PROTOCOL);
        uint8_t position_out;
        for (position_out = 0; position_out < sizeof(packet_t); position_out++) Serial.write(IN_ALIGN_COMMUNICATION_PROTOCOL);
      }
    }

    // exec instruction if exec_time == 0
     // else if instruction buffer is not full store instruction
    if (current_instruction.data.exec_time == 0) {
      exec_instruction(&current_instruction, false);
    }
    else {
      add_input_packet(&current_instruction);
    }
  }
  
  // write one element of the output buffer
  if (output_buffer_read && Serial.availableForWrite()){
    send_output_ring_buffer_first();
  } 

  // send error if output buffer is full, DoS
  //else if (output_ring_buffer_start == (output_ring_buffer_next_free + 1) % OUTPUT_BUFFER_SIZE) {
  //  error_message_bypass_buffer(OUT_ERROR,OUT_ERROR_OUTPUT_FULL,output_ring_buffer_next_free);
  //} 

  if (loop_runs_without_gpio_interrups > 0){
    loop_runs_without_gpio_interrups--;
    if (loop_runs_without_gpio_interrups == 0){
      enable_gpio_interrupt();
    }
  }
}