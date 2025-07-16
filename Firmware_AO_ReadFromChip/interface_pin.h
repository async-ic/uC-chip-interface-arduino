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

#ifndef INTERFACE_PIN_H
#define INTERFACE_PIN_H
#include <cstdint>
#include <Arduino.h>
#include <chrono>
#include "core_ring_buffer.h"
#include "uc_boards.h"



/*
 the Pin helper tracks the usage and configuration off all the pins.
 and changes thier state
*/

// the usage of pins, to avoid double use
extern volatile bool input_pin_active[NUMBER_OF_DIGITAL_PINS];
extern volatile bool output_pin_active[NUMBER_OF_DIGITAL_PINS];

/*
 register a pin as input, changes the direction of the uC acordingly
 retuns true if sucessfull, false if pin was previously registered for something else.
*/
bool reserve_input_pin(uint8_t id, uint8_t from_instruction = OUT_ERROR_PIN_ALREADY_INUSE);

/*
 register a pin as output, changes the direction of the uC acordingly
 retuns true if sucessfull, false if pin was previously registered for something else.
*/
bool reserve_output_pin(uint8_t id, uint8_t from_instruction = OUT_ERROR_PIN_ALREADY_INUSE);

/*
 processes all pin related configuration instructions, exspects a PIN_CONF package.
 it registers recording interrups for input pins, and registeres pin usage.
*/
void configure_pin(uint8_t instruction, uint8_t data);

/*
 checks if the pin is a valid output pin and sets the pin to that value.
 it can also write output pins that are defind by other interfaces like SPI or AER
*/
void set_pin(uint8_t id, uint32_t data);

/*
 checks if the pin is a valid input or output pin and read the pin to that value.
 it can also read output pins that are defind by other interfaces like SPI or AER
*/
void read_pin(uint8_t id);

void debug_pin(uint8_t id, uint8_t value = HIGH);

void pin_ISR(uint8_t id);
void pin_ISR0();
void pin_ISR1();
void pin_ISR2();
void pin_ISR3();
void pin_ISR4();
void pin_ISR5();
void pin_ISR6();
void pin_ISR7();
void pin_ISR8();
void pin_ISR9();
void pin_ISR10();
void pin_ISR11();
void pin_ISR12();
void pin_ISR13();
void pin_ISR14();
void pin_ISR15();
void pin_ISR16();
void pin_ISR17();
void pin_ISR18();
void pin_ISR19();
void pin_ISR20();
void pin_ISR21();
void pin_ISR22();
void pin_ISR23();
void pin_ISR24();
void pin_ISR25();
void pin_ISR26();
void pin_ISR27();
void pin_ISR28();
void pin_ISR29();
void pin_ISR30();
void pin_ISR31();
void pin_ISR32();
void pin_ISR33();
void pin_ISR34();
void pin_ISR35();
void pin_ISR36();
void pin_ISR37();
void pin_ISR38();
void pin_ISR39();
void pin_ISR40();
void pin_ISR41();
void pin_ISR42();
void pin_ISR43();
void pin_ISR44();
void pin_ISR45();
void pin_ISR46();
void pin_ISR47();
void pin_ISR48();
void pin_ISR49();
void pin_ISR50();
void pin_ISR51();
void pin_ISR52();
void pin_ISR53();
void pin_ISR54();

#endif