/*
    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
    Copyright (C) 2022-2023 Ole Richter - University of Groningen
    Copyright (C) 2022 Benjamin Hucko

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

#include "interface_pin.h"


volatile bool input_pin_active[NUMBER_OF_DIGITAL_PINS] = {};
volatile bool output_pin_active[NUMBER_OF_DIGITAL_PINS] = {};

bool reserve_input_pin(uint8_t id, uint8_t from_instruction) {
  if (id >= NUMBER_OF_DIGITAL_PINS){
    error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,from_instruction,id);
    return false;
  }
  noInterrupts();
  if (input_pin_active[id] || output_pin_active[id]) {
    interrupts();
    error_message(OUT_ERROR_PIN_ALREADY_INUSE,from_instruction,id);
    return false;
  }
  else{
    input_pin_active[id] = true;
  }
  interrupts();
  return true;
}

bool reserve_output_pin(uint8_t id, uint8_t from_instruction) {
  if (id >= NUMBER_OF_DIGITAL_PINS){
    error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,from_instruction,id);
    return false;
  }
  noInterrupts();
  if (input_pin_active[id] || output_pin_active[id]) {
    interrupts();
    error_message(OUT_ERROR_PIN_ALREADY_INUSE,from_instruction,id);
    return false;
  }
  else{
    output_pin_active[id] = true;
  }
  interrupts();
  return true;
}

void configure_pin(uint8_t instruction, uint8_t data){

  switch (instruction){
    case CONF_OUTPUT:
      if (reserve_output_pin(data)){
        pinMode(data, OUTPUT);
      }
      break;
    case CONF_INPUT:
          if (reserve_input_pin(data)){
        pinMode(data, INPUT);
        switch(data){
          case 0: attachInterrupt(digitalPinToInterrupt(data), pin_ISR0, CHANGE); break;
          case 1: attachInterrupt(digitalPinToInterrupt(data), pin_ISR1, CHANGE); break;
          case 2: attachInterrupt(digitalPinToInterrupt(data), pin_ISR2, CHANGE); break;
          case 3: attachInterrupt(digitalPinToInterrupt(data), pin_ISR3, CHANGE); break;
          case 4: attachInterrupt(digitalPinToInterrupt(data), pin_ISR4, CHANGE); break;
          case 5: attachInterrupt(digitalPinToInterrupt(data), pin_ISR5, CHANGE); break;
          case 6: attachInterrupt(digitalPinToInterrupt(data), pin_ISR6, CHANGE); break;
          case 7: attachInterrupt(digitalPinToInterrupt(data), pin_ISR7, CHANGE); break;
          case 8: attachInterrupt(digitalPinToInterrupt(data), pin_ISR8, CHANGE); break;
          case 9: attachInterrupt(digitalPinToInterrupt(data), pin_ISR9, CHANGE); break;
          case 10: attachInterrupt(digitalPinToInterrupt(data), pin_ISR10, CHANGE); break;
          case 11: attachInterrupt(digitalPinToInterrupt(data), pin_ISR11, CHANGE); break;
          case 12: attachInterrupt(digitalPinToInterrupt(data), pin_ISR12, CHANGE); break;
          case 13: attachInterrupt(digitalPinToInterrupt(data), pin_ISR13, CHANGE); break;
          case 14: attachInterrupt(digitalPinToInterrupt(data), pin_ISR14, CHANGE); break;
          case 15: attachInterrupt(digitalPinToInterrupt(data), pin_ISR15, CHANGE); break;
          case 16: attachInterrupt(digitalPinToInterrupt(data), pin_ISR16, CHANGE); break;
          case 17: attachInterrupt(digitalPinToInterrupt(data), pin_ISR17, CHANGE); break;
          case 18: attachInterrupt(digitalPinToInterrupt(data), pin_ISR18, CHANGE); break;
          case 19: attachInterrupt(digitalPinToInterrupt(data), pin_ISR19, CHANGE); break;
          case 20: attachInterrupt(digitalPinToInterrupt(data), pin_ISR20, CHANGE); break;
          case 21: attachInterrupt(digitalPinToInterrupt(data), pin_ISR21, CHANGE); break;
          case 22: attachInterrupt(digitalPinToInterrupt(data), pin_ISR22, CHANGE); break;
          case 23: attachInterrupt(digitalPinToInterrupt(data), pin_ISR23, CHANGE); break;
          case 24: attachInterrupt(digitalPinToInterrupt(data), pin_ISR24, CHANGE); break;
          case 25: attachInterrupt(digitalPinToInterrupt(data), pin_ISR25, CHANGE); break;
          case 26: attachInterrupt(digitalPinToInterrupt(data), pin_ISR26, CHANGE); break;
          case 27: attachInterrupt(digitalPinToInterrupt(data), pin_ISR27, CHANGE); break;
          case 28: attachInterrupt(digitalPinToInterrupt(data), pin_ISR28, CHANGE); break;
          case 29: attachInterrupt(digitalPinToInterrupt(data), pin_ISR29, CHANGE); break;
          case 30: attachInterrupt(digitalPinToInterrupt(data), pin_ISR30, CHANGE); break;
          case 31: attachInterrupt(digitalPinToInterrupt(data), pin_ISR31, CHANGE); break;
          case 32: attachInterrupt(digitalPinToInterrupt(data), pin_ISR32, CHANGE); break;
          case 33: attachInterrupt(digitalPinToInterrupt(data), pin_ISR33, CHANGE); break;
          case 34: attachInterrupt(digitalPinToInterrupt(data), pin_ISR34, CHANGE); break;
          case 35: attachInterrupt(digitalPinToInterrupt(data), pin_ISR35, CHANGE); break;
          case 36: attachInterrupt(digitalPinToInterrupt(data), pin_ISR36, CHANGE); break;
          case 37: attachInterrupt(digitalPinToInterrupt(data), pin_ISR37, CHANGE); break;
          case 38: attachInterrupt(digitalPinToInterrupt(data), pin_ISR38, CHANGE); break;
          case 39: attachInterrupt(digitalPinToInterrupt(data), pin_ISR39, CHANGE); break;
          case 40: attachInterrupt(digitalPinToInterrupt(data), pin_ISR40, CHANGE); break;
          case 41: attachInterrupt(digitalPinToInterrupt(data), pin_ISR41, CHANGE); break;
          case 42: attachInterrupt(digitalPinToInterrupt(data), pin_ISR42, CHANGE); break;
          case 43: attachInterrupt(digitalPinToInterrupt(data), pin_ISR43, CHANGE); break;
          case 44: attachInterrupt(digitalPinToInterrupt(data), pin_ISR44, CHANGE); break;
          case 45: attachInterrupt(digitalPinToInterrupt(data), pin_ISR45, CHANGE); break;
          case 46: attachInterrupt(digitalPinToInterrupt(data), pin_ISR46, CHANGE); break;
          case 47: attachInterrupt(digitalPinToInterrupt(data), pin_ISR47, CHANGE); break;
          case 48: attachInterrupt(digitalPinToInterrupt(data), pin_ISR48, CHANGE); break;
          case 49: attachInterrupt(digitalPinToInterrupt(data), pin_ISR49, CHANGE); break;
          case 50: attachInterrupt(digitalPinToInterrupt(data), pin_ISR50, CHANGE); break;
          case 51: attachInterrupt(digitalPinToInterrupt(data), pin_ISR51, CHANGE); break;
          case 52: attachInterrupt(digitalPinToInterrupt(data), pin_ISR52, CHANGE); break;
          case 53: attachInterrupt(digitalPinToInterrupt(data), pin_ISR53, CHANGE); break;
          case 54: attachInterrupt(digitalPinToInterrupt(data), pin_ISR54, CHANGE); break;
          default:
            error_message(OUT_ERROR_UNKNOWN_CONFIGURATION,instruction,data);
            return;
        }
      }
      break;
    default:
      error_message(OUT_ERROR_UNKNOWN_CONFIGURATION,instruction,data);
      return;
    }
    // send confirmation
    send_config(IN_CONF_PIN,instruction,data);
}

/**
 * 
*/
void set_pin(uint8_t id, uint32_t data){
  if (output_pin_active[id]){
      if (data) {
          digitalWrite(id, HIGH);
        } else {
          digitalWrite(id, LOW);
        }
      // send confirmation
      send_pin(IN_PIN,id,data,true);
  }
  else{
    error_message(OUT_ERROR_PIN_NOT_CONFIGURED,IN_PIN,id);
  }
}

void read_pin(uint8_t id){
  if (output_pin_active[id] || input_pin_active[id]){
      uint8_t data = digitalRead(id);
      if (data) send_pin(OUT_PIN_HIGH, id, data);
      else send_pin(OUT_PIN_LOW, id, data);
      // send confirmation
      send_pin(IN_PIN_READ,id,0,true);
  }
  else{
    error_message(OUT_ERROR_PIN_NOT_CONFIGURED,IN_PIN_READ,id);
  }
}

void debug_pin(uint8_t id, uint8_t value){
  if (reserve_output_pin(id)){
        pinMode(id, OUTPUT);
        digitalWrite(id, value);
      }
}

void pin_ISR(uint8_t id) {
  #if defined(ARDUINO_TEENSY41)
  uint8_t data = digitalReadFast(id);
  #else
  uint8_t data = digitalRead(id);
  #endif
  if (data) send_pin(OUT_PIN_HIGH, id, data);
  else send_pin(OUT_PIN_LOW, id, data);
}

void pin_ISR0() { pin_ISR(0); }
void pin_ISR1() { pin_ISR(1); }
void pin_ISR2() { pin_ISR(2); }
void pin_ISR3() { pin_ISR(3); }
void pin_ISR4() { pin_ISR(4); }
void pin_ISR5() { pin_ISR(5); }
void pin_ISR6() { pin_ISR(6); }
void pin_ISR7() { pin_ISR(7); }
void pin_ISR8() { pin_ISR(8); }
void pin_ISR9() { pin_ISR(9); }
void pin_ISR10() { pin_ISR(10); }
void pin_ISR11() { pin_ISR(11); }
void pin_ISR12() { pin_ISR(12); }
void pin_ISR13() { pin_ISR(13); }
void pin_ISR14() { pin_ISR(14); }
void pin_ISR15() { pin_ISR(15); }
void pin_ISR16() { pin_ISR(16); }
void pin_ISR17() { pin_ISR(17); }
void pin_ISR18() { pin_ISR(18); }
void pin_ISR19() { pin_ISR(19); }
void pin_ISR20() { pin_ISR(20); }
void pin_ISR21() { pin_ISR(21); }
void pin_ISR22() { pin_ISR(22); }
void pin_ISR23() { pin_ISR(23); }
void pin_ISR24() { pin_ISR(24); }
void pin_ISR25() { pin_ISR(25); }
void pin_ISR26() { pin_ISR(26); }
void pin_ISR27() { pin_ISR(27); }
void pin_ISR28() { pin_ISR(28); }
void pin_ISR29() { pin_ISR(29); }
void pin_ISR30() { pin_ISR(30); }
void pin_ISR31() { pin_ISR(31); }
void pin_ISR32() { pin_ISR(32); }
void pin_ISR33() { pin_ISR(33); }
void pin_ISR34() { pin_ISR(34); }
void pin_ISR35() { pin_ISR(35); }
void pin_ISR36() { pin_ISR(36); }
void pin_ISR37() { pin_ISR(37); }
void pin_ISR38() { pin_ISR(38); }
void pin_ISR39() { pin_ISR(39); }
void pin_ISR40() { pin_ISR(40); }
void pin_ISR41() { pin_ISR(41); }
void pin_ISR42() { pin_ISR(42); }
void pin_ISR43() { pin_ISR(43); }
void pin_ISR44() { pin_ISR(44); }
void pin_ISR45() { pin_ISR(45); }
void pin_ISR46() { pin_ISR(46); }
void pin_ISR47() { pin_ISR(47); }
void pin_ISR48() { pin_ISR(48); }
void pin_ISR49() { pin_ISR(49); }
void pin_ISR50() { pin_ISR(50); }
void pin_ISR51() { pin_ISR(51); }
void pin_ISR52() { pin_ISR(52); }
void pin_ISR53() { pin_ISR(53); }
void pin_ISR54() { pin_ISR(54); }