/*
    This file is part of the Firmware project to interface with small Neuromorphic chips
    Copyright (C) 2022 Ole Richter - University of Groningen

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

#ifndef DATATYPES_H
#define DATATYPES_H
#include <cstdint>


/*
 the packet header are byte that identifies the instruction to be executed by the uC
 order is as follows
  - general commands
  - write to interface PIN, AER, SPI
  - configure Interfaces
*/
enum inPacketHeader : uint8_t {
  /*
   read all availiable output packets out of the output buffer, and clear the buffer afterwards
   uses data:
    - value is ignored
  */  
  IN_READ = 0U,

  /*
   use
  */  
  IN_READ_LAST = 4U,

  /*
   set the execution time, if set to 0 recording and exec are halted
   if set to 1 (or bigger) exec and recoding will be started from timestep 1 (or bigger)
   uses data
    - value is the time current to be set, role over of system time is not handled
  */
  IN_SET_TIME = 1U, 
  /*
   return the current execution time to the output buffer
   uses data
    - value is ignored
  */
  IN_READ_TIME = 2U, 
  /*
   read all availiable input packets out of the input buffer that have not been processed
   uses data
    - value is ignored
  */  
  IN_READ_INSTRUCTIONS = 3U,
  /*
   send a modify output pin command
   uses pin
    - pin is the pin number
    - value is 0 or 1 for low or high
  */
  IN_PIN = 10U,
  /*
   send a read input pin command
   uses pin
    - pin is the pin number
    - value is ignored
  */
  IN_READ_PIN = 11U,
  /*
   sends an 8bit word on the SPI0 interface
   uses data8
   - value is the 8bit to be send
  */
  IN_SPI0 = 20U,
  /*
   sends an 8bit word on the SPI1 interface
   uses data8
   - value is the 8bit to be send
  */
  IN_SPI1 = 21U,
  /*
   sends an 8bit word on the SPI2 interface
   uses data8
   - value is the 8bit to be send
  */
  IN_SPI2 = 22U,
    /*
   sends an 32bit word on the SPI0 interface
   uses data32
   - value is the 8bit to be send
  */
  IN_SPI0_32 = 23U,
  /*
   sends an 32bit word on the SPI1 interface
   uses data32
   - value is the 8bit to be send
  */
  IN_SPI1_32 = 24U,
  /*
   sends an 32bit word on the SPI2 interface
   uses data32
   - value is the 8bit to be send
  */
  IN_SPI2_32 = 25U,
  /*
   sends an 0-32bit word on the AER_TO_CHIP0 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_AER_TO_CHIP0 = 30U,
  /*
   sends an 0-32bit word on the AER_TO_CHIP1 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_AER_TO_CHIP1 = 31U,
  /*
   sends an 0-32bit word on the AER_TO_CHIP2 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_AER_TO_CHIP2 = 32U,
  /*
   sends an 0-32bit word on the AER_TO_CHIP3 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_AER_TO_CHIP3 = 33U,
  /*
   sends an 0-32bit word on the AER_TO_CHIP4 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_AER_TO_CHIP4 = 34U,
  /*
   sends an 0-32bit word on the AER_TO_CHIP5 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_AER_TO_CHIP5 = 35U,
  /*
   sends an 0-32bit word on the AER_TO_CHIP6 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_AER_TO_CHIP6 = 36U,
  /*
   sends an 0-32bit word on the AER_TO_CHIP7 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_AER_TO_CHIP7 = 37U,
  /*
   sets the pin configuration
   uses config
    - config/sub header is the config state to be applied the pin
    - value is the pin id
  */
  IN_CONF_PIN = 50U,
  /*
   sets the spi0 configuration
   uses config
    - config/sub header is the config state to be applied the spi
    - value is used according to the config sub header
  */
  IN_CONF_SPI0 = 60U,
   /*
  sets the spi1 configuration
   uses config
    - config/sub header is the config state to be applied the spi
    - value is used according to the config sub header
  */
  IN_CONF_SPI1 = 61U, 
   /*
   sets the spi2 configuration
   uses config
    - config/sub header is the config state to be applied the spi
    - value is used according to the config sub header
  */
  IN_CONF_SPI2 = 62U, 
  /*
   sets the AER_TO_CHIP0 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_TO_CHIP0 = 70U,
  /*
   sets the AER_TO_CHIP1 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_TO_CHIP1 = 71U,
  /*
   sets the AER_TO_CHIP2 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_TO_CHIP2 = 72U,
  /*
   sets the AER_TO_CHIP3 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_TO_CHIP3 = 73U,
  /*
   sets the AER_TO_CHIP4 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_TO_CHIP4 = 74U,
  /*
   sets the AER_TO_CHIP5 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_TO_CHIP5 = 75U,
  /*
   sets the AER_TO_CHIP6 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_TO_CHIP6 = 76U,
  /*
   sets the AER_TO_CHIP7 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_TO_CHIP7 = 77U,
  /*
   sets the AER_FROM_CHIP0 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_FROM_CHIP0 = 80U,
  /*
   sets the AER_FROM_CHIP1 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_FROM_CHIP1 = 81U,
  /*
   sets the AER_FROM_CHIP2 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_FROM_CHIP2 = 82U,
  /*
   sets the AER_FROM_CHIP3 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_FROM_CHIP3 = 83U, 
  /*
   sets the AER_FROM_CHIP4 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_FROM_CHIP4 = 84U,
  /*
   sets the AER_FROM_CHIP5 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_FROM_CHIP5 = 85U, 
  /*
   sets the AER_FROM_CHIP6 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_FROM_CHIP6 = 86U,
  /*
   sets the AER_FROM_CHIP7 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_AER_FROM_CHIP7 = 87U, 
  
};

/*
 the packet header is byte that identifies the information to be read by the host
 order is as follows
  - general info
  - value is the word to be send, incomming data from PIN, AER, SPI
  - error messages
  - other transaction messages
*/
enum outPacketHeader : uint8_t {
  /*
   responce to the READ_TIME packet
   uses data
    - exec_time the current run time 
    - value the system_time (without run time offset)
  */
  OUT_TIME = 100U,
  /*
   After a input pin change this records the change
   uses pin
    - exec_time the time the change occured
    - pin the pin id
    - value the new state
  */
  OUT_PIN = 110U,
  /*
   responce to the IN_SPI0 packet
   uses data8
    - exec_time the current run time 
    - value the 8bit word that was read on the SPI
  */
  OUT_SPI0 = 120U,
  /*
   responce to the IN_SPI1 packet
   uses data8
    - exec_time the current run time 
    - value the 8bit word that was read on the SPI
  */
  OUT_SPI1 = 121U,
  /*
   responce to the IN_SPI2 packet
   uses data8
    - exec_time the current run time 
    - value the 8bit word that was read on the SPI
  */
  OUT_SPI2 = 122U,
  /*
   responce to the IN_SPI0_32 packet
   uses data32
    - exec_time the current run time 
    - value the 32bit word that was read on the SPI
  */
  OUT_SPI0_32 = 123U,
  /*
   responce to the IN_SPI1_32 packet
   uses data32
    - exec_time the current run time 
    - value the 32bit word that was read on the SPI
  */
  OUT_SPI1_32 = 124U,
  /*
   responce to the IN_SPI2_32 packet
   uses data32
    - exec_time the current run time 
    - value the 32bit word that was read on the SPI
  */
  OUT_SPI2_32 = 125U,
  /*
   a event was reseved on AER_FROM_CHIP0
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_AER_FROM_CHIP0 = 130U,
  /*
   a event was reseved on AER_FROM_CHIP1
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_AER_FROM_CHIP1 = 131U,
  /*
   a event was reseved on AER_FROM_CHIP2
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_AER_FROM_CHIP2 = 132U,
  /*
   a event was reseved on AER_FROM_CHIP3
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_AER_FROM_CHIP3 = 133U,
  /*
   a event was reseved on AER_FROM_CHIP4
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_AER_FROM_CHIP4 = 134U,
  /*
   a event was reseved on AER_FROM_CHIP5
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_AER_FROM_CHIP5 = 135U,
  /*
   a event was reseved on AER_FROM_CHIP6
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_AER_FROM_CHIP6 = 136U,
  /*
   a event was reseved on AER_FROM_CHIP7
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_AER_FROM_CHIP7 = 137U,
  /*
   
   uses 
  */
  OUT_ERROR = 200U,
  /*
   
   uses 
  */
  OUT_ERROR_PIN_ALREADY_INUSE = 201U,
  /*
   
   uses 
  */
  OUT_ERROR_PIN_NOT_CONFIGURED = 202U,
  /*
   
   uses 
  */
  OUT_ERROR_INPUT_FULL = 203U,
  /*
   
   uses 
  */
  OUT_ERROR_OUTPUT_FULL = 204U,
  /*
   
   uses 
  */
  OUT_ERROR_INTERFACE_ALREADY_ACTIVE = 205U,
  /*
   
   uses 
  */
  OUT_ERROR_UNKNOWN_INSTRUCTION = 206U,
  /*
   
   uses 
  */
  OUT_ERROR_INTERFACE_NOT_ACTIVE = 207U,
  /*
   
   uses 
  */
  OUT_ERROR_UNKNOWN_CONFIGURATION = 208U,
  /*
   
   uses 
  */
  OUT_ERROR_AER_HS_TIMEOUT = 209U,
 /*
   this packet is used to destinguish between an empty and a full buffer
   uses data
    - value is 1 so ignore
  */
  OUT_BUFFER_LAST_READ = 250U,

  /*
   
   header used to return success status after pin confiuration.
  */
  OUT_SUCCESS_PIN_CONFIGURED = 240U,
 
  
};
  
enum confPacketHeader : uint8_t {
  /*
   set an interface to active, after activation the pins/width cant be changed anymore
   works for spi and aer, activation will fail if pins are already used for other interfaces
   @TODO deactivation not implemented yet sould be new header for compatebility
   uses config
    - value is ignored
  */
  CONF_ACTIVE = 60U,
  /*
   set a pin to output to be able to write on it
   uses config
    - value is ID of pin
  */
  CONF_OUTPUT = 61U,
  /*
   set a pin to input, it registers an interupt service 
   to record all incomming changes
   uses config
    - value is ID of pin
  */
  CONF_INPUT = 62U,
  /*
   sets the Req pin for the given AER interface
   uses config
       - value is ID of pin
  */
  CONF_REQ = 70U,
  /*
   sets the Ack pin for the given AER interface
   uses config
       - value is ID of pin
  */
  CONF_ACK = 71U,
  /*
   sets the bit width of the given AER interface
   uses config
    - value is width 0-32
  */
  CONF_WIDTH = 72U,
  /*
   set the delay on the request line
   uses config
    - value the delay in multiple of 20ns
  */
  CONF_REQ_DELAY = 73U,
  /*
   if the req and ack are active low or high
   uses config
    - value 0 for active high, value 1 for active low
  */
  CONF_HS_ACTIVE_LOW = 74U,
  /*
   if the data channels are active low or high
   uses config
    - value 0 for active high, value 1 for active low
  */
  CONF_DATA_ACTIVE_LOW = 75U,
  /*
   setting the pin for AER data channel X
   uses config
    - value is the pin ID
  */
  CONF_CHANNEL0 = 0U,
  CONF_CHANNEL1 = 1U,
  CONF_CHANNEL2 = 2U,
  CONF_CHANNEL3 = 3U,
  CONF_CHANNEL4 = 4U,
  CONF_CHANNEL5 = 5U,
  CONF_CHANNEL6 = 6U,
  CONF_CHANNEL7 = 7U,
  CONF_CHANNEL8 = 8U,
  CONF_CHANNEL9 = 9U,
  CONF_CHANNEL10 = 10U,
  CONF_CHANNEL11 = 11U,
  CONF_CHANNEL12 = 12U,
  CONF_CHANNEL13 = 13U,
  CONF_CHANNEL14 = 14U,
  CONF_CHANNEL15 = 15U,
  CONF_CHANNEL16 = 16U,
  CONF_CHANNEL17 = 17U,
  CONF_CHANNEL18 = 18U,
  CONF_CHANNEL19 = 19U,
  CONF_CHANNEL20 = 20U,
  CONF_CHANNEL21 = 21U,
  CONF_CHANNEL22 = 22U,
  CONF_CHANNEL23 = 23U,
  CONF_CHANNEL24 = 24U,
  CONF_CHANNEL25 = 25U,
  CONF_CHANNEL26 = 26U,
  CONF_CHANNEL27 = 27U,
  CONF_CHANNEL28 = 28U,
  CONF_CHANNEL29 = 29U,
  CONF_CHANNEL30 = 30U,
  CONF_CHANNEL31 = 31U,
  CONF_CHANNEL32 = 32U,


};

/*
the struct packet_t is 9 byte big,
the 9 bytes can be utilised in different ways:
*/

/*
the data32 packet format: 9 bytes
*/
#pragma pack(push,1)
struct data32_t{
  uint8_t header;
  uint32_t exec_time;
  uint32_t value;
};
#pragma pack(pop)

/*
the data8 packet format: 6 bytes
*/
#pragma pack(push,1)
struct data8_t{
  uint8_t header;
  uint32_t exec_time;
  uint8_t value;
};
#pragma pack(pop)

/*
the config packet format: 7 bytes
*/
#pragma pack(push,1)
struct config_t{
  uint8_t header;
  uint32_t exec_time;
  uint8_t config_header;
  uint8_t value;
};
#pragma pack(pop)

/*
the pin packet format: 7 bytes
*/
#pragma pack(push,1)
struct pin_t {
  uint8_t header;
  uint32_t exec_time;
  uint8_t id;
  uint8_t value;
};
#pragma pack(pop)

/*
the error packet format: 6 bytes
*/
#pragma pack(push,1)
struct error_t{
  uint8_t header;
  uint8_t org_header;
  uint32_t value;
};
#pragma pack(pop)

/*
the struct packet_t is 9 byte big,
the 9 bytes can be utilised in different ways
*/
#pragma pack(push,1)
union packet_t{
  uint8_t bytes[1+4+4];
  data32_t data;
  data8_t data_8;
  config_t config;
  pin_t pin;
  error_t error;
};
#pragma pack(pop)

/*
helper function to deep copy packets from volotile to not volotile and reverse
*/
packet_t copy_packet(volatile packet_t* in);

void copy_packet(packet_t* in, volatile packet_t* out);

#endif