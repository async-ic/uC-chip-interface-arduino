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

#ifndef INTERFACE_I2C_H
#define INTERFACE_I2C_H

#include <Arduino.h>
#include <Wire.h>
#include "core_ring_buffer.h"
#include "interface_pin.h"
#include "datatypes.h"
#include "uc_boards.h"


class Interface_i2c {
    /*

    */

    public:
    /*
        the configure class method handles incomming configuration packets, and also instatiactes the class on activation
        requires:
            id - the id of the interface the config is intended for
            config - the config sub header that determines what is configured
            data - the associated data to the configuration (if required by the config otherwise ignored)
    */
    static void configure(uint8_t id, uint8_t config, uint8_t data);

    /*
        the static configuration stoarge for the interfaces, 
        as the configuration is send in packets that can appear in random order or be incomplete
        they are stored staticly in the class untill the interface is activated
    */
    static volatile uint8_t width[I2C_NUMBER_OF_INTERFACES];
    static volatile uint8_t byte_order[I2C_NUMBER_OF_INTERFACES];
    static volatile bool active[I2C_NUMBER_OF_INTERFACES];
    static volatile uint32_t frequency[I2C_NUMBER_OF_INTERFACES];
    /*
        the pointers to the interface instantces after activation.
    */
    static volatile Interface_i2c* inst[I2C_NUMBER_OF_INTERFACES];

    static void process_packet(uint8_t id,uint8_t device_address, uint8_t register_address, uint8_t data_ms, uint8_t data_ls);

    //-----------------------------------------------------------------------------------------------------------------------------------
    // Class constructor;
    //-----------------------------------------------------------------------------------------------------------------------------------
    Interface_i2c(uint8_t id, uint32_t freqency = 100000U);


    //----------------------------------------------------------------------------------------------------------------------------------
    // number of bytes can be 1-2s
    //----------------------------------------------------------------------------------------------------------------------------------    
    void write(uint8_t device_address, uint8_t register_address, uint8_t data_ms, uint8_t data_ls) volatile;

    //----------------------------------------------------------------------------------------------------------------------------------
    // number of bytes can be 1-4
    //----------------------------------------------------------------------------------------------------------------------------------
    void read(uint8_t device_address, uint8_t register_address, uint8_t number_of_bytes) volatile;
    uint16_t read_return(uint8_t device_address, uint8_t register_address) volatile;

    // ---------------------------------------------------- Declaring private methods --------------------------------------------------
    protected:

    uint8_t _id;
    uint32_t _frequency;
    #if defined(TEENSYDUINO)
    TwoWire* _i2c;
    #else
    arduino::TwoWire* _i2c;
    #endif
};

#endif