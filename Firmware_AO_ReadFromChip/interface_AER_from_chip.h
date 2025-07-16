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

#ifndef AER_from_chip_H
#define AER_from_chip_H

#include <Arduino.h>
#include "misc_functions.h"
#include "core_ring_buffer.h"
#include "interface_pin.h"
#include "datatypes.h"
#include "uc_boards.h"
#include "interface_AER_to_chip.h"

class AER_from_chip {
    /*
        the AER_from_chip class handles the flexible and runtime assignable 4 phase handshake protocols for reciving data with the uC.
        all the configuration is static and the moment the interface is activated an instance of this class is created and the 
        refenence stored in the static array AER_from_chip.
        the following settings can be configured for each interface:
        data pins locations, width from 0-32, a multiple of 20ns delay on the request line, if the data or handshake signals are active low 
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
    static volatile uint8_t data_pins[8][32];
    static volatile uint8_t data_width[8];
    static volatile uint8_t req_pin[8];
    static volatile uint8_t ack_pin[8];
    static volatile uint8_t req_delay[8];
    static volatile bool hs_lowactive[8];
    static volatile bool data_lowactive[8];
    static volatile bool active[8];
    static volatile uint8_t type[8];
    static volatile uint8_t port[8];
    /*
        the pointers to the interface instantces after activation.
    */
    static volatile AER_from_chip* inst[8];

    //-----------------------------------------------------------------------------------------------------------------------------------
    // Class constructor; initialises the AER_from_chip object and sets up the relevant pins on Teensy
    //-----------------------------------------------------------------------------------------------------------------------------------
    AER_from_chip(uint8_t id, uint8_t reqPin, uint8_t ackPin, volatile uint8_t dataPins[], uint8_t numDataPins, uint8_t delay = 0, 
                bool activeLow = false, bool dataActiveLow = false, uint8_t type=ASYNC_4Phase_Chigh_Dhigh);


    //----------------------------------------------------------------------------------------------------------------------------------
    // reqRead: Reads REQ pin state
    //----------------------------------------------------------------------------------------------------------------------------------    
    bool reqRead() volatile;

    //----------------------------------------------------------------------------------------------------------------------------------
    // ackWrite: Writes to ACK pin
    //----------------------------------------------------------------------------------------------------------------------------------
    void ackWrite(bool val) volatile;

    //----------------------------------------------------------------------------------------------------------------------------------
    // recordEvent: Records output events as they occur
    //----------------------------------------------------------------------------------------------------------------------------------
    void recordEvent() volatile;

    //----------------------------------------------------------------------------------------------------------------------------------
    // handshake: Executes REQ/ACK handshake between Teensy and chip
    //----------------------------------------------------------------------------------------------------------------------------------
    void handshake();

    // ---------------------------------------------------- Declaring private methods --------------------------------------------------
    protected:
    
    uint32_t getData() volatile;
    //----------------------------------------------------------------------------------------------------------------------------------
    // setupPins: Sets up the relevant pins for communication
    //----------------------------------------------------------------------------------------------------------------------------------
    bool setupPins();

    // --------------------------------------------------- Declaring private variables -------------------------------------------------

    uint8_t _reqPin;
    uint8_t _ackPin;
    volatile uint8_t* _dataPins;
    uint8_t _numDataPins;
    uint8_t _delay;
    bool _handshakeActiveLow;
    bool _dataActiveLow;
    uint8_t _id;
    uint8_t _type;

};

void aer_ISR(uint8_t id);
void aer0_ISR();
void aer1_ISR();
void aer2_ISR();
void aer3_ISR();
void aer4_ISR();
void aer5_ISR();
void aer6_ISR();
void aer7_ISR();

#endif