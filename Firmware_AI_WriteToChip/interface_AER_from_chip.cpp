/*
    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
    Copyright (C) 2022 Matteo Cartiglia - University of Zurich
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
#include <Arduino.h>
#include "interface_AER_from_chip.h"
#include "misc_functions.h"
#include "interface_i2c.h"
#include <Wire.h>

// reserve and clear the memory of the static global variables.
// see header for description
volatile uint8_t AER_from_chip::data_pins[8][32] = {};
volatile uint8_t AER_from_chip::data_width[8] = {};
volatile uint8_t AER_from_chip::req_pin[8] = {};
volatile uint8_t AER_from_chip::ack_pin[8] = {};
volatile uint8_t AER_from_chip::req_delay[8] = {};
volatile bool AER_from_chip::hs_lowactive[8] = {};
volatile bool AER_from_chip::data_lowactive[8] = {};
volatile bool AER_from_chip::active[8] = {};
volatile uint8_t AER_from_chip::type[8] = {};
volatile uint8_t AER_from_chip::port[8] = {};
volatile AER_from_chip* AER_from_chip::inst[8] = {};

// handles incomming configuraion packets
void AER_from_chip::configure(uint8_t id, uint8_t config, uint8_t data){
  // check if the ID is valid otherwise throw error
  if (id >= 8) {
    error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_CONF_ASYNC_FROM_CHIP0,id);
    return;
  }
  uint8_t interface;
  switch(id){
    case 0: interface = IN_CONF_ASYNC_FROM_CHIP0; break;
    case 1: interface = IN_CONF_ASYNC_FROM_CHIP1; break;
    case 2: interface = IN_CONF_ASYNC_FROM_CHIP2; break;
    case 3: interface = IN_CONF_ASYNC_FROM_CHIP3; break;
    case 4: interface = IN_CONF_ASYNC_FROM_CHIP4; break;
    case 5: interface = IN_CONF_ASYNC_FROM_CHIP5; break;
    case 6: interface = IN_CONF_ASYNC_FROM_CHIP6; break;
    case 7: interface = IN_CONF_ASYNC_FROM_CHIP7; break;
    default:interface = OUT_ERROR; break;
  }
  // handle the different config sub headers
  switch (config){
    // activate the interface and reserve the pins
    case CONF_ACTIVE:
      AER_from_chip::inst[id] = new AER_from_chip(id,
                                      AER_from_chip::req_pin[id],
                                      AER_from_chip::ack_pin[id],
                                      AER_from_chip::data_pins[id],
                                      AER_from_chip::data_width[id],
                                      AER_from_chip::req_delay[id],
                                      AER_from_chip::hs_lowactive[id],
                                      AER_from_chip::data_lowactive[id],
                                      AER_from_chip::type[id]);
      if (AER_from_chip::active[id]){
        // send confirmation
        send_config(interface,config,AER_from_chip::active[id]);
      }
      return;
    // configure the type (done via class switching for speed!)
    case CONF_TYPE:
      // replace by class
      if (data == ASYNC_4Phase_Clow_Dhigh) AER_from_chip::hs_lowactive[id]=1;
      AER_from_chip::type[id] = data;
      if (data == ASYNC_4Phase_MCP23017){
        // if (!Interface_i2c::active[0]) {
        //   Interface_i2c::configure(0,CONF_BYTE_ORDER,0U);
        //   Interface_i2c::configure(0,CONF_WIDTH,2U);
        //   Interface_i2c::inst[0] = new Interface_i2c(0,100000U);
        // } else {
        //   error_message(OUT_ERROR_INTERFACE_ALREADY_ACTIVE,IN_CONF_I2C0,0,interface);
        //   return;
        // } 
      }
      break;
    // configure the location of the request pin
    case CONF_REQ:
      AER_from_chip::req_pin[id] = data;
      break;
    // configure the location of the ack pin
    case CONF_ACK:
      AER_from_chip::ack_pin[id] = data;
      break;
    // configure how many data channels are used in the AER interface
    case CONF_WIDTH:
      // throw error if interface is to wide
      if (data > 32) {
        error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,interface,id,config);
        AER_from_chip::data_width[id] = 32;
        // send confirmation
        send_config(interface,config,32);
        return;
      }
      else {
        AER_from_chip::data_width[id] = data;
      }
      break;
    // configure the delay on the req line
    case CONF_REQ_DELAY:
      AER_from_chip::req_delay[id] = data;
      break;
    // conf if handshake signals are active low or not
    default:
      if (config < 32){
        AER_from_chip::data_pins[id][config] = data;
      }
      // throw and error if the config header is not implemented
      else {
        error_message(OUT_ERROR_UNKNOWN_CONFIGURATION,interface,id,config);
        return;
      }
  }
  //send confirmation
  send_config(interface,config,data);
}


//---------------------------------------------------------------------------------------------------------------------------------------
// AER_from_chip constructor
//---------------------------------------------------------------------------------------------------------------------------------------

AER_from_chip::AER_from_chip(uint8_t id, uint8_t reqPin, uint8_t ackPin, volatile uint8_t dataPins[], uint8_t numDataPins, uint8_t delay, bool handshakeActiveLow, bool dataActiveLow, uint8_t type) {
  _reqPin = reqPin;
  _ackPin = ackPin;
  _dataPins = dataPins;
  _numDataPins = numDataPins;
  _delay = delay;
  _type = type;
  _handshakeActiveLow = handshakeActiveLow;
  _dataActiveLow = dataActiveLow;
  _id = id;

  if (id >= 8) {
    error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_CONF_ASYNC_FROM_CHIP0,id);
    return;
  }

  if (setupPins()){ 
    switch(_id){
      case 0: 
        attachInterrupt(digitalPinToInterrupt(reqPin), aer0_ISR, CHANGE); 
        AER_from_chip::active[id] = true;
        break;
      case 1: 
        attachInterrupt(digitalPinToInterrupt(reqPin), aer1_ISR, CHANGE); 
        AER_from_chip::active[id] = true;
        break;
      case 2: 
        attachInterrupt(digitalPinToInterrupt(reqPin), aer2_ISR, CHANGE); 
        AER_from_chip::active[id] = true;
        break;
      case 3: 
        attachInterrupt(digitalPinToInterrupt(reqPin), aer3_ISR, CHANGE); 
        AER_from_chip::active[id] = true;
        break;
      case 4: 
        attachInterrupt(digitalPinToInterrupt(reqPin), aer4_ISR, CHANGE); 
        AER_from_chip::active[id] = true;
        break;
      case 5: 
        attachInterrupt(digitalPinToInterrupt(reqPin), aer5_ISR, CHANGE); 
        AER_from_chip::active[id] = true;
        break;
      case 6: 
        attachInterrupt(digitalPinToInterrupt(reqPin), aer6_ISR, CHANGE); 
        AER_from_chip::active[id] = true;
        break;
      case 7: 
        attachInterrupt(digitalPinToInterrupt(reqPin), aer7_ISR, CHANGE); 
        AER_from_chip::active[id] = true;
        break;
      default:
        error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_CONF_ASYNC_TO_CHIP0,id);
    }
  } 
}

//---------------------------------------------------------------------------------------------------------------------------------------
// reqRead: Reads REQ pin state
//---------------------------------------------------------------------------------------------------------------------------------------

bool  AER_from_chip::reqRead() volatile {
  #if defined(TEENSYDUINO)
  return digitalReadFast(_reqPin)^_handshakeActiveLow;
  #else
  return digitalRead(_reqPin)^_handshakeActiveLow;
  #endif
  
}

//---------------------------------------------------------------------------------------------------------------------------------------
// ackWrite: Writes to ACK pin
//---------------------------------------------------------------------------------------------------------------------------------------

void  AER_from_chip::ackWrite(bool val) volatile {
  #if defined(TEENSYDUINO)
  digitalWriteFast(_ackPin, val^_handshakeActiveLow);
  #else
  digitalWrite(_ackPin, val^_handshakeActiveLow);
  #endif
}


//----------------------------------------------------------------------------------------------------------------------------------
// recordEvent: Records output events as they occur
//----------------------------------------------------------------------------------------------------------------------------------
void AER_from_chip::recordEvent() volatile {
  uint8_t interface;
    switch(_id){
      case 0: interface = OUT_ASYNC_FROM_CHIP0; break;
      case 1: interface = OUT_ASYNC_FROM_CHIP1; break;
      case 2: interface = OUT_ASYNC_FROM_CHIP2; break;
      case 3: interface = OUT_ASYNC_FROM_CHIP3; break;
      case 4: interface = OUT_ASYNC_FROM_CHIP4; break;
      case 5: interface = OUT_ASYNC_FROM_CHIP5; break;
      case 6: interface = OUT_ASYNC_FROM_CHIP6; break;
      case 7: interface = OUT_ASYNC_FROM_CHIP7; break;
      default:interface = OUT_ERROR; break;
    }    
    send_data32(interface, getData());
}

//----------------------------------------------------------------------------------------------------------------------------------
// handshake: Executes REQ/ACK handshake between Teensy and ALIVE
//----------------------------------------------------------------------------------------------------------------------------------
void AER_from_chip::handshake() {
  if(_handshakeActiveLow) {
    if (!reqRead()) {
      ackWrite(0); //@TODO replace with direct pin access
    }
    else if (reqRead()) {
      ackWrite(1); //@TODO replace with direct pin access
    }

  }

  else if(!_handshakeActiveLow) {
    if (!reqRead()) {
      ackWrite(0); //@TODO replace with direct pin access
    }

    else if (reqRead()) {
      ackWrite(1); //@TODO replace with direct pin access
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------------------
// setupPins: Sets up the relevant pins for communication
//----------------------------------------------------------------------------------------------------------------------------------

bool AER_from_chip::setupPins() {
  if (reserve_input_pin(_reqPin)) pinMode(_reqPin, INPUT);
  else return false;
  if (reserve_output_pin(_ackPin)) pinMode(_ackPin, OUTPUT);
  else return false;
  if (_type == ASYNC_4Phase_MCP23017){
    Wire.begin();
    Wire.setClock(400000);
    return true;
  }
  else {
    for(int i = 0; i < _numDataPins; i++) {
      if (reserve_input_pin(_dataPins[i])) pinMode(_dataPins[i], INPUT);
      else return false;
    }
    return true;
  }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// getData: Retrieves event
//---------------------------------------------------------------------------------------------------------------------------------------

uint32_t AER_from_chip::getData() volatile{
  uint32_t data = 0;
  if (_delay) {
    delay20ns(_delay);
  }
  if (_type == ASYNC_4Phase_MCP23017){

      uint8_t interface;
      switch(_id){
        case 0: interface = OUT_ASYNC_FROM_CHIP0; break;
        case 1: interface = OUT_ASYNC_FROM_CHIP1; break;
        case 2: interface = OUT_ASYNC_FROM_CHIP2; break;
        case 3: interface = OUT_ASYNC_FROM_CHIP3; break;
        case 4: interface = OUT_ASYNC_FROM_CHIP4; break;
        case 5: interface = OUT_ASYNC_FROM_CHIP5; break;
        case 6: interface = OUT_ASYNC_FROM_CHIP6; break;
        case 7: interface = OUT_ASYNC_FROM_CHIP7; break;
        default:interface = OUT_ERROR; break;
      }
      uint8_t count_shift = 0;
      uint8_t transmission_success = 0;
      Wire.beginTransmission((uint8_t) 32U);
      Wire.write((uint8_t) 18U);
      
      transmission_success = Wire.endTransmission(false);
      if(transmission_success>0){
        error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,interface,transmission_success,IN_I2C2);
        return 0;
      }
     
      transmission_success = Wire.requestFrom((uint8_t) 32U,2,true);
      if(transmission_success != 2){
        error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,interface,transmission_success,OUT_I2C2);
        return 0;
      }

      data |= Wire.read()<<8*count_shift;
      count_shift++;
      data |= Wire.read()<<8*count_shift;
      count_shift++;

      Wire.beginTransmission((uint8_t) 33U);
      Wire.write((uint8_t) 18U);
      transmission_success = Wire.endTransmission(false);
      if(transmission_success>0){
        error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,interface,transmission_success,IN_I2C2);
        return 0;
      }
      
      transmission_success = Wire.requestFrom((uint8_t) 33U,2,true);
      if(transmission_success != 2){
        error_message(OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY,interface,transmission_success,OUT_I2C2);
        return 0;
      }
      data |= Wire.read()<<8*count_shift;
      count_shift++;
      data |= Wire.read()<<8*count_shift;
      count_shift++;
      
  
    // data = Interface_i2c::inst[0]->read_return(32U,18U);
    // data |= (Interface_i2c::inst[0]->read_return(33U,18U)<<16);
    return data;

  }

  else{
    for (int i = 0; i < _numDataPins; i++) {
      #if defined(TEENSYDUINO)
      data |= digitalReadFast(_dataPins[i]) << i; // @todo read with direct pin access
      #else
      data |= digitalRead(_dataPins[i]) << i;
      #endif
    }
    if (_dataActiveLow) {
      return ~data;
    }
    else {
      return data;
    }
  }
}


void aer_ISR(uint8_t id) {
  if (AER_from_chip::inst[id]->reqRead()) {
      if ( offset_time != 0 ) { 
        AER_from_chip::inst[id]->recordEvent();
      }   
      AER_from_chip::inst[id]->ackWrite(1);
    }
  else if (!AER_from_chip::inst[id]->reqRead()) {
      AER_from_chip::inst[id]->ackWrite(0);
  }
}

void aer0_ISR() { aer_ISR(0); }
void aer1_ISR() { aer_ISR(1); }
void aer2_ISR() { aer_ISR(2); }
void aer3_ISR() { aer_ISR(3); }
void aer4_ISR() { aer_ISR(4); }
void aer5_ISR() { aer_ISR(5); }
void aer6_ISR() { aer_ISR(6); }
void aer7_ISR() { aer_ISR(7); }
