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
#include "misc_funtions.h"
#include "core_ring_buffer.h"
#include "Interface_pin.h"
#include "datatypes.h"
#include "uc_boards.h"

enum Async_from_chip_types : uint8_t {
    AER_FULL_CUSTOM = 0U,
    //AER_TEENSY_BANK = 1U,
    AER_MCP23017 = 2U,
};

class Async {
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
    static volatile bool active[8];
    static volatile uint8_t type[8];
    static volatile uint8_t port[8];
    /*
        the pointers to the interface instantces after activation.
    */
    static volatile Async_from_chip* inst[8];

    static void async_ISR(uint8_t id);
    static void async0_ISR();
    static void async1_ISR();
    static void async2_ISR();
    static void async3_ISR();
    static void async4_ISR();
    static void async5_ISR();
    static void async6_ISR();
    static void async7_ISR();

    //-----------------------------------------------------------------------------------------------------------------------------------
    // Class constructor; initialises the AER_from_chip object and sets up the relevant pins on Teensy
    //-----------------------------------------------------------------------------------------------------------------------------------
    AER_from_chip(uint8_t id, uint8_t reqPin, uint8_t ackPin, uint8_t delay = 0);


    virtual void request_isr() = 0;
    
    protected:
    
    virtual uint32_t record_data() = 0 volatile;
    //----------------------------------------------------------------------------------------------------------------------------------
    // setupPins: Sets up the relevant pins for communication
    //----------------------------------------------------------------------------------------------------------------------------------
    bool setup_hs_pins();
    bool setup_data_pins();

    void exec_4_phase_active_high();
    void exec_4_phase_active_low();
    void exec_2_phase();


    // --------------------------------------------------- Declaring private variables -------------------------------------------------

    uint8_t _reqPin;
    uint8_t _ackPin;
    volatile uint8_t* _dataPins;
    uint8_t _numDataPins;
    uint8_t _delay;
    uint8_t _id;

};

class Async_to_chip : Async
{
private:
    /* data */
public:
    interface_async(/* args */);
    ~interface_async();
};

class Async_from_chip : Async
{
private:
    /* data */
public:
    interface_async(/* args */);
    ~interface_async();
};

interface_async::interface_async(/* args */)
{
}

interface_async::~interface_async()
{
}




#endif