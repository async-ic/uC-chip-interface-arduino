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
#ifndef AER_to_chip_H
#define AER_to_chip_H

#include <Arduino.h>
#include "core_ring_buffer.h"
#include "interface_pin.h"
#include "datatypes.h"
#include "misc_functions.h"

#define AER_HANDSHAKE_TIMEOUT         1

enum ASYNC_TYPE : uint8_t {
    ASYNC_4Phase_Chigh_Dhigh = 0U,
    ASYNC_4Phase_Clow_Dhigh = 1U,
    ASYNC_2Phase = 10U,
    ASYNC_4Phase_MCP23017 = 20U,
};


class AER_to_chip
{
    // ------------------------------------------ Declaring class constructor and public methods ----------------------------------------
    public:

    static void configure(uint8_t id, uint8_t config, uint8_t data);
    static void send_packet(uint8_t id, uint32_t data, uint8_t header);

    static volatile uint8_t data_pins[8][32];
    static volatile uint8_t data_width[8];
    static volatile uint8_t req_pin[8];
    static volatile uint8_t ack_pin[8];
    static volatile uint8_t req_delay[8];
    static volatile uint8_t type[8];
    static volatile bool hs_lowactive[8];
    static volatile bool data_lowactive[8];
    static volatile bool active[8];
    static volatile AER_to_chip* inst[8];

    //-----------------------------------------------------------------------------------------------------------------------------------
    // Class constructor; initialises the AER_to_chip object and sets up the relevant pins on Teensy
    //-----------------------------------------------------------------------------------------------------------------------------------
    AER_to_chip(uint8_t id, uint8_t reqPin, uint8_t ackPin, volatile uint8_t dataPins[], uint8_t numDataPins, uint8_t delay = 0, bool activeLow = false);

    //----------------------------------------------------------------------------------------------------------------------------------
    // dataWrite: Executes REQ/ACK handshake and writes  to ALIVE
    //----------------------------------------------------------------------------------------------------------------------------------
    bool dataWrite(uint32_t data) volatile;


    // ---------------------------------------------------- Declaring private methods --------------------------------------------------

    private:

    //----------------------------------------------------------------------------------------------------------------------------------
    // setupPins: Sets up the relevant pins for communication
    //----------------------------------------------------------------------------------------------------------------------------------
    bool setupPins();

    //----------------------------------------------------------------------------------------------------------------------------------
    // ackRead: Reads ACK pin state
    //---------------------------------------------------------------------------------------------------------------------------------- 
    bool ackRead() volatile;

    //----------------------------------------------------------------------------------------------------------------------------------
    // reqWrite: Writes to REQ pin
    //----------------------------------------------------------------------------------------------------------------------------------
    void reqWrite(bool val) volatile;
    
    //----------------------------------------------------------------------------------------------------------------------------------
    // setData: Write data to  pins
    //----------------------------------------------------------------------------------------------------------------------------------
    void setData(uint32_t data) volatile;


    // --------------------------------------------------- Declaring private variables -------------------------------------------------

    uint8_t _reqPin;
    uint8_t _ackPin;
    volatile uint8_t* _dataPins;
    uint8_t _numDataPins;
    uint8_t _delay;
    bool _activeLow;
    uint8_t _id;
};



#endif