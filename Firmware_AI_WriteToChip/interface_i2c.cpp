/*
    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
    Copyright (C) 2023 Ole Richter - University of Groningen

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
#include <Arduino.h>
#include "interface_i2c.h"
#include <Wire.h>


// reserve and clear the memory of the static global variables.
// see header for description
volatile uint8_t Interface_i2c::width[I2C_NUMBER_OF_INTERFACES] = {};
volatile uint32_t Interface_i2c::frequency[I2C_NUMBER_OF_INTERFACES] = {};
volatile uint8_t Interface_i2c::byte_order[I2C_NUMBER_OF_INTERFACES] = {};
volatile bool Interface_i2c::active[I2C_NUMBER_OF_INTERFACES] = {};

volatile Interface_i2c* Interface_i2c::inst[I2C_NUMBER_OF_INTERFACES] = {};

// handles incomming configuraion packets
void Interface_i2c::configure(uint8_t id, uint8_t config, uint8_t data){
  // check if the ID is valid otherwise throw error
  if (id >= I2C_NUMBER_OF_INTERFACES) {
    error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_CONF_I2C0,id);
    return;
  }
  uint8_t interface;
  switch(id){
    case 0: interface = IN_CONF_I2C0; break;
    case 1: interface = IN_CONF_I2C1; break;
    case 2: interface = IN_CONF_I2C2; break;
    default:interface = IN_CONF_I2C0; break; // should never happen
  }
  // handle the different config sub headers
  switch (config){
    // activate the interface and reserve the pins
    case CONF_ACTIVE:
          Interface_i2c::inst[id] = new Interface_i2c(id,Interface_i2c::frequency[id]); 
      if (Interface_i2c::active[id]){
        // send confirmation
        send_config(interface,config,Interface_i2c::active[id]);
      }
      return;
    case CONF_WIDTH:
      // throw error if interface is to wide
      if (data > 2 || data < 1) {
        error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,interface,id,config);
        Interface_i2c::width[id] = 1;
        // send confirmation
        send_config(interface,config,1);
        return;
      }
      else Interface_i2c::width[id] = data;
      break;
    case CONF_BYTE_ORDER:
      Interface_i2c::byte_order[id] = (data > 0);
      send_config(interface,config,(data > 0));
      break;
    case CONF_SPEED_CLASS:
      switch (data){
        default:
        case 0:
          Interface_i2c::frequency[id] = 100000;
          send_config(interface,config,0);
          break;
        case 1:
          Interface_i2c::frequency[id] = 400000;
          send_config(interface,config,data);
          break;
        case 2:
          Interface_i2c::frequency[id] = 1000000;
          send_config(interface,config,data);
          break;
        case 3:
          Interface_i2c::frequency[id] = 3400000;
          send_config(interface,config,data);
          break;
        case 4:
          Interface_i2c::frequency[id] = 10000;
          send_config(interface,config,data);
          break;
      }
      
      break;
    
    default:
      error_message(OUT_ERROR_UNKNOWN_CONFIGURATION,interface,id,config);
      return;
      
  }
  //send confirmation
  send_config(interface,config,data);
}


void Interface_i2c::process_packet(uint8_t id,uint8_t device_address_8, uint8_t register_address, uint8_t data_ms, uint8_t data_ls){
  bool read = device_address_8 & 254;
  uint8_t device_address = device_address_8 >> 1;

  if (id >= I2C_NUMBER_OF_INTERFACES) {
    error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_I2C0,id,CONF_ACTIVE);
    return;
  }
  else if (!Interface_i2c::active[id]){
    switch (id){
        default:
        case 0: error_message(OUT_ERROR_INTERFACE_NOT_ACTIVE,IN_I2C0,id); break;
        case 1: error_message(OUT_ERROR_INTERFACE_NOT_ACTIVE,IN_I2C1,id); break;
        case 2: error_message(OUT_ERROR_INTERFACE_NOT_ACTIVE,IN_I2C2,id); break;
      }
  }
  else{
    if (read) Interface_i2c::inst[id]->read(device_address,register_address,data_ls);
    else Interface_i2c::inst[id]->write(device_address,register_address,data_ls,data_ms);
  }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// Interface_i2c constructor
//---------------------------------------------------------------------------------------------------------------------------------------

Interface_i2c::Interface_i2c(uint8_t id, uint32_t frequency) {
  _id = id;
  if (frequency == 0) _frequency = 100000;
  else _frequency = frequency;
  if (id >= I2C_NUMBER_OF_INTERFACES) {
    error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_CONF_I2C0,id);
  }
  else {
    switch(_id){
      case 0: 
        if(reserve_output_pin(I2C_SCL_PORT, IN_CONF_I2C0) && reserve_input_pin(I2C_SDA_PORT, IN_CONF_I2C0)){
          _i2c = &Wire;
          _i2c->setClock(_frequency);
          _i2c->setTimeout(1000);
          _i2c->begin();
          Interface_i2c::active[id] = true;
        } 
        break;
      #if (I2C_NUMBER_OF_INTERFACES > 1)
      case 1: 
        if(reserve_output_pin(I2C_SCL1_PORT, IN_CONF_I2C1) && reserve_input_pin(I2C_SDA1_PORT, IN_CONF_I2C1)){
          _i2c = &Wire1;
          _i2c->setClock(_frequency);
          _i2c->setTimeout(1000);
          _i2c->begin();
          Interface_i2c::active[id] = true;
        } 
        break;
      #endif
      #if (I2C_NUMBER_OF_INTERFACES > 2)
      case 2: 
        if(reserve_output_pin(I2C_SCL2_PORT, IN_CONF_I2C2) && reserve_input_pin(I2C_SDA2_PORT, IN_CONF_I2C2)){
          _i2c = &Wire2;
          _i2c->setClock(_frequency);
          _i2c->setTimeout(1000);
          _i2c->begin();
          Interface_i2c::active[id] = true;
        } 
        break;
      #endif
      default:
        error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_CONF_I2C0,id,CONF_ACTIVE);
    }
  }
}

void Interface_i2c::write(uint8_t device_address, uint8_t register_address, uint8_t data_ls, uint8_t data_ms) volatile {
  uint8_t result = 0;
  if (_i2c != NULL){
    noInterrupts();
    _i2c->beginTransmission(device_address);
    if (Interface_i2c::width[_id] != 0) _i2c->write(register_address);
    if (Interface_i2c::width[_id] == 2 && Interface_i2c::byte_order[_id]) _i2c->write(data_ms);
    _i2c->write(data_ls);
    if (Interface_i2c::width[_id] == 2 && !Interface_i2c::byte_order[_id]) _i2c->write(data_ms);
    result = _i2c->endTransmission(true);
    interrupts();
    if(result>0){
          switch (_id){
              case 0: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C0,result); break;
              case 1: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C1,result); break;
              case 2: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C2,result); break;
            }
        }
    switch (_id){
      case 0: send_data_i2c(IN_I2C0,device_address<<1,register_address,data_ls,data_ms); break;
      case 1: send_data_i2c(IN_I2C1,device_address<<1,register_address,data_ls,data_ms); break;
      case 2: send_data_i2c(IN_I2C2,device_address<<1,register_address,data_ls,data_ms); break;
    }
    
  }
  else{
    //this makes no sense here
      error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_I2C0,_id,CONF_ACTIVE);
  }
}

void Interface_i2c::read(uint8_t device_address, uint8_t register_address, uint8_t number_of_bytes) volatile{
  uint8_t result = 0;
  if (number_of_bytes > 32) {
    switch (_id){
      default:
      case 0: error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_I2C0,number_of_bytes,CONF_INPUT); break;
      case 1: error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_I2C1,number_of_bytes,CONF_INPUT); break;
      case 2: error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_I2C2,number_of_bytes,CONF_INPUT); break;
    }
  return;
  }
  if (_i2c != NULL){
      noInterrupts();
      uint8_t byte_mulitply = Interface_i2c::width[_id];
      if (byte_mulitply != 0) {
      _i2c->beginTransmission(device_address);
      _i2c->write(register_address);
      result = _i2c->endTransmission(false);
      if(result>0){
          switch (_id){
              case 0: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C0,result); break;
              case 1: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C1,result); break;
              case 2: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C2,result); break;
            }
        } 
      }
      else byte_mulitply = 1;
      result = _i2c->requestFrom(device_address,number_of_bytes*byte_mulitply,true);
      interrupts();
      if(result!=byte_mulitply){
          switch (_id){
              case 0: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C0,0); break;
              case 1: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C1,0); break;
              case 2: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C2,0); break;
            }
        }
      if (_i2c->available() == number_of_bytes*byte_mulitply) {
        uint8_t position = 0;
        for (position = 0; position < number_of_bytes; position++){
          uint8_t data_ls = 0;
          uint8_t data_ms = 0;
          if (byte_mulitply == 2 && !Interface_i2c::byte_order[_id]){
            data_ls = _i2c->read();
            data_ms = _i2c->read();
          }
          else if (byte_mulitply == 2 && Interface_i2c::byte_order[_id]){
            data_ms = _i2c->read();
            data_ls = _i2c->read();
          }
          else {
            data_ls = _i2c->read();
          }
          switch (_id){
            case 0: send_data_i2c(IN_I2C0,(device_address<<1)+1,register_address,data_ls,data_ms); break;
            case 1: send_data_i2c(IN_I2C1,(device_address<<1)+1,register_address,data_ls,data_ms); break;
            case 2: send_data_i2c(IN_I2C2,(device_address<<1)+1,register_address,data_ls,data_ms); break;
          }
        }
      }
      
    else{
      // this does not make sense here
      error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_I2C0,_id,CONF_ACTIVE);
    }
  }
}

uint16_t Interface_i2c::read_return(uint8_t device_address, uint8_t register_address) volatile{
  uint16_t data = 0;
  uint8_t result = 0;
  if (_i2c != NULL){
      noInterrupts();
      uint8_t byte_mulitply = Interface_i2c::width[_id];
      if (byte_mulitply != 0) {
        _i2c->beginTransmission(device_address);
        _i2c->write(register_address);
        result = _i2c->endTransmission();
        if(result>0){
          switch (_id){
              case 0: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C0,result); break;
              case 1: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C1,result); break;
              case 2: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C2,result); break;
            }
        } 
      }
      else byte_mulitply = 1;
      result = _i2c->requestFrom(device_address,byte_mulitply,true);
      interrupts();
      if(result!=byte_mulitply){
          switch (_id){
              case 0: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C0,0); break;
              case 1: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C1,0); break;
              case 2: error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,IN_I2C2,0); break;
            }
        }
      if (_i2c->available() == byte_mulitply) {
        if (byte_mulitply == 2 && !Interface_i2c::byte_order[_id]){
          data = _i2c->read();
          data |= (_i2c->read()<<8);
        }
        else if (byte_mulitply == 2 && Interface_i2c::byte_order[_id]){
          data = (_i2c->read()<<8);
          data |= _i2c->read();
        }
        else {
          data = _i2c->read();
        }
        return data;
      }
    else{
      // this does not make sense here
      error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_I2C0,_id,CONF_ACTIVE);
    }
  }
}
