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
#include "core_instruction_exec.h"
#include "uc_boards.h"
#include <Arduino.h>
//#include <avr/wdt.h>

volatile bool exec_active = false;
volatile IntervalTimer myTimer = IntervalTimer();

void(* reset_uC) (void) = 0; // restart program (config reset/not hardware reset)

void exec_instruction(packet_t* instruction, bool is_ISR_call) {
  switch(instruction->data.header){
  case IN_READ: send_output_ring_buffer(); break;
  case IN_READ_LAST: send_output_ring_buffer_last(); break;
  case IN_READ_INSTRUCTIONS: send_input_ring_buffer(); break;
  case IN_FREE_INSTRUCTION_SPOTS: send_input_ring_buffer_free_spots(); break;
  case IN_SET_TIME: set_time_offset(instruction->data.value); break;
  case IN_READ_TIME: read_time(is_ISR_call); break;
  case IN_PIN: set_pin(instruction->pin.id,instruction->pin.value); break;
  case IN_PIN_READ: read_pin(instruction->pin.id); break;
  case IN_SPI0: Interface_spi::send_packet(0,instruction->data.value); break;  
  case IN_SPI1: Interface_spi::send_packet(1,instruction->data.value); break;  
  case IN_SPI2: Interface_spi::send_packet(2,instruction->data.value); break;  
  case IN_I2C0: Interface_i2c::process_packet(0,instruction->data_i2c.component_address,instruction->data_i2c.register_address,instruction->data_i2c.value_ms,instruction->data_i2c.value_ls); break;  
  case IN_I2C1: Interface_i2c::process_packet(1,instruction->data_i2c.component_address,instruction->data_i2c.register_address,instruction->data_i2c.value_ms,instruction->data_i2c.value_ls); break;  
  case IN_I2C2: Interface_i2c::process_packet(2,instruction->data_i2c.component_address,instruction->data_i2c.register_address,instruction->data_i2c.value_ms,instruction->data_i2c.value_ls); break;  
  case IN_ASYNC_TO_CHIP0: AER_to_chip::send_packet(0,instruction->data.value, instruction->data.header); break;  
  case IN_ASYNC_TO_CHIP1: AER_to_chip::send_packet(1,instruction->data.value, instruction->data.header); break;
  case IN_ASYNC_TO_CHIP2: AER_to_chip::send_packet(2,instruction->data.value, instruction->data.header); break;  
  case IN_ASYNC_TO_CHIP3: AER_to_chip::send_packet(3,instruction->data.value, instruction->data.header); break;
  case IN_ASYNC_TO_CHIP4: AER_to_chip::send_packet(4,instruction->data.value, instruction->data.header); break;  
  case IN_ASYNC_TO_CHIP5: AER_to_chip::send_packet(5,instruction->data.value, instruction->data.header); break;
  case IN_ASYNC_TO_CHIP6: AER_to_chip::send_packet(6,instruction->data.value, instruction->data.header); break;  
  case IN_ASYNC_TO_CHIP7: AER_to_chip::send_packet(7,instruction->data.value, instruction->data.header); break;  
  case IN_CONF_PIN: configure_pin(instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_SPI0: Interface_spi::configure(0,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_SPI1: Interface_spi::configure(1,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_SPI2: Interface_spi::configure(2,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_I2C0: Interface_i2c::configure(0,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_I2C1: Interface_i2c::configure(1,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_I2C2: Interface_i2c::configure(2,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_TO_CHIP0: AER_to_chip::configure(0,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_TO_CHIP1: AER_to_chip::configure(1,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_TO_CHIP2: AER_to_chip::configure(2,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_TO_CHIP3: AER_to_chip::configure(3,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_TO_CHIP4: AER_to_chip::configure(4,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_TO_CHIP5: AER_to_chip::configure(5,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_TO_CHIP6: AER_to_chip::configure(6,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_TO_CHIP7: AER_to_chip::configure(7,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_FROM_CHIP0: AER_from_chip::configure(0,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_FROM_CHIP1: AER_from_chip::configure(1,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_FROM_CHIP2: AER_from_chip::configure(2,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_FROM_CHIP3: AER_from_chip::configure(3,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_FROM_CHIP4: AER_from_chip::configure(4,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_FROM_CHIP5: AER_from_chip::configure(5,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_FROM_CHIP6: AER_from_chip::configure(6,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_ASYNC_FROM_CHIP7: AER_from_chip::configure(7,instruction->config.config_header,instruction->config.value); break;
  case IN_CONF_READ_ON_REQUEST: set_read_on_request(instruction->config.value); break;

  // the mainheader is now being used to split the instruction into functions, but we need to use the mainheader to identify which neuron was meant. 
  case SAVE_MAPPING_TABLE_MIN ... SAVE_MAPPING_TABLE_MAX:
    addRecurrencyConnection(instruction->data.header, instruction->config.config_header, instruction->config.value);   
    break;

  case IN_RESET://wdt_enable(WDTO_15MS); 
    reset_uC(); 
    break;

  case OUT_ALIGN_SUCCESS_VERSION: 
    error_message_bypass_buffer(OUT_ALIGN_SUCCESS_VERSION, VERSION_MAJOR, VERSION_PATCH, VERSION_MINOR); 
    break;

  default:   
    if (is_ISR_call) 
      error_message(OUT_ERROR_UNKNOWN_INSTRUCTION,instruction->data.header,instruction->data.value);
    else 
      error_message_bypass_buffer(OUT_ERROR_UNKNOWN_INSTRUCTION,instruction->data.header,instruction->data.value);
    break;
  }
}

void exec_timed_commands(){
  noInterrupts();
  uint8_t attempt = 0;
  if (exec_active) return;
  exec_active = true;
  uint32_t current_time = micros() - offset_time; // time does not move forward in interupts  
  
  packet_t instruction = copy_packet(&input_ring_buffer[input_ring_buffer_start]);
  
  while (
    (input_ring_buffer_start % INPUT_BUFFER_SIZE != input_ring_buffer_next_free) &&
     instruction.data.exec_time <= current_time ) {
    interrupts();
    exec_instruction(&instruction,true);
    noInterrupts();
    input_ring_buffer_start = (input_ring_buffer_start + 1) % INPUT_BUFFER_SIZE;
    instruction = copy_packet(&input_ring_buffer[input_ring_buffer_start]);
  }
  exec_active = false;
  interrupts();
}

void set_time_offset(uint32_t offset){
  if (offset){
    offset_time = micros() + offset;
    myTimer.begin(exec_timed_commands, EXEC_PRESISION);
    //send confirmation
    send_data32(IN_SET_TIME,offset,true);
  }
  else{
    myTimer.end();  
    input_ring_buffer_start = input_ring_buffer_next_free;
    //send confirmation
    send_data32(IN_SET_TIME,offset,true);
    offset_time = 0;  
  }

}

void read_time(bool is_ISR_call){
  send_data32(IN_READ_TIME,micros(),true);
}
