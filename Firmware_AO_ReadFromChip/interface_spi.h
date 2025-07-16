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

#ifndef INTERFACE_SPI_H
#define INTERFACE_SPI_H
#include <cstdint>
#include <Arduino.h>
#include "core_ring_buffer.h"
#include "interface_pin.h"
#include <SPI.h>

class Interface_spi {
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
    static void configure(uint8_t id, uint8_t config_option, uint8_t data);

    /*
        the static configuration stoarge for the interfaces, 
        as the configuration is send in packets that can appear in random order or be incomplete
        they are stored staticly in the class untill the interface is activated
    */
    static volatile uint8_t width[SPI_NUMBER_OF_INTERFACES];
    static volatile uint32_t frequency[SPI_NUMBER_OF_INTERFACES];
    static volatile uint8_t mode[SPI_NUMBER_OF_INTERFACES];
    static volatile bool byte_order[SPI_NUMBER_OF_INTERFACES];
    static volatile bool active[SPI_NUMBER_OF_INTERFACES];

    /*
        the pointers to the interface instantces after activation.
    */
    static volatile Interface_spi* inst[SPI_NUMBER_OF_INTERFACES];

    static void send_packet(uint8_t id, uint32_t data);

    //-----------------------------------------------------------------------------------------------------------------------------------
    // Class constructor;
    //-----------------------------------------------------------------------------------------------------------------------------------
    Interface_spi(uint8_t id, uint32_t frequency = 100000U, uint8_t width = 1, uint8_t mode = 0, bool byte_order = false);


    //----------------------------------------------------------------------------------------------------------------------------------
    // number of bytes can be 1-2s
    //----------------------------------------------------------------------------------------------------------------------------------    
    void write(uint32_t data) volatile;

    // ---------------------------------------------------- Declaring private methods --------------------------------------------------
    protected:

    uint8_t _id;
    uint8_t _mode;
    uint8_t _width;
    bool _byte_order;
    uint32_t _frequency;
    #if defined(TEENSYDUINO)
    SPIClass *_spi;
    #else
    arduino::SPIClass *_spi;
    #endif
    

};


#endif