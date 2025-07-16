/*
    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
    Copyright (C) 2022-2023 Ole Richter - University of Groningen

    Last Updated: 2025/03/04 - Vincent Jassies
    Added recurrency mapping headers

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
  @todo rename to IN_RUN_EXPERIMENT
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
     this packet is used to request how many free spots there are in the instruction ring buffer
  */
  IN_FREE_INSTRUCTION_SPOTS = 5U,
  /*
  @todo move to sub config
     legacy mode: turn off automatic sending of the output buffer
     uses config
     - value 1 for read on request (legacy)
     - value 0 for continous read (default)
  */
  IN_CONF_READ_ON_REQUEST = 6U,
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
  IN_PIN_READ = 11U,
  /*
   sends an 8/16/32bbit word on the SPI0 interface
   uses data32
   - value is the  to be send
  */
  IN_SPI0 = 20U,
  /*
   sends an 8/16/32bbit word on the SPI1 interface
   uses data32
   - value is the  to be send
  */
  IN_SPI1 = 21U,
  /*
   sends an 8/16/32bit word on the SPI2 interface
   uses data32
   - value is the to be send
  */
  IN_SPI2 = 22U,

 /*
   sends an 8 or 16 bit word or recives a specified number of bytes on the I2C interface
   uses data_i2c
   - device_address is the 7bit (MS) address of sets the ASYNC_FROM_CHIP7 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub headerss
   - value_ms is the MS 8bit to be send
   - value_ls is the LS 8bit to be send or the number of bytes to read
  */
  IN_I2C0 = 25U,
 /*
   sends an 8 or 16 bit word or recives a specified number of bytes on the I2C interface
   uses data_i2c
   - device_address is the 7bit (MS) address of the device and the LSB indicates Read(1)/Write(0) following the i2c standard
   - register_address is the 8bit register address
   - value_ms is the MS 8bit to be send
   - value_ls is the LS 8bit to be send or the number of bytes to read
  */
  IN_I2C1 = 26U,
  /*
   sends an 8 or 16 bit word or recives a specified number of bytes on the I2C interface
   uses data_i2c
   - device_address is the 7bit (MS) address of the device and the LSB indicates Read(1)/Write(0) following the i2c standard
   - register_address is the 8bit register address
   - value_ms is the MS 8bit to be send
   - value_ls is the LS 8bit to be send or the number of bytes to read
  */
  IN_I2C2 = 27U,
  /*
   sends an 0-32bit word on the ASYNC_TO_CHIP0 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_ASYNC_TO_CHIP0 = 30U,
  /*
   sends an 0-32bit word on the ASYNC_TO_CHIP1 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_ASYNC_TO_CHIP1 = 31U,
  /*
   sends an 0-32bit word on the ASYNC_TO_CHIP2 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_ASYNC_TO_CHIP2 = 32U,
  /*
   sends an 0-32bit word on the ASYNC_TO_CHIP3 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_ASYNC_TO_CHIP3 = 33U,
  /*
   sends an 0-32bit word on the ASYNC_TO_CHIP4 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_ASYNC_TO_CHIP4 = 34U,
  /*
   sends an 0-32bit word on the ASYNC_TO_CHIP5 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_ASYNC_TO_CHIP5 = 35U,
  /*
   sends an 0-32bit word on the ASYNC_TO_CHIP6 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_ASYNC_TO_CHIP6 = 36U,
  /*
   sends an 0-32bit word on the ASYNC_TO_CHIP7 interface
   uses data
    - value is the word to be send, if the width is configured to >32 the MS bits are ignored
  */
  IN_ASYNC_TO_CHIP7 = 37U,
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
   sets the i2c0 configuration
   uses config
    - config/sub header is the config state to be applied the spi
    - value is used according to the config sub header
  */
  IN_CONF_I2C0 = 65U,
   /*
  sets the i2c1 configuration
   uses config
    - config/sub header is the config state to be applied the spi
    - value is used according to the config sub header
  */
  IN_CONF_I2C1 = 66U, 
   /*
   sets the i2c2 configuration
   uses config
    - config/sub header is the config state to be applied the spi
    - value is used according to the config sub header
  */
  IN_CONF_I2C2 = 67U, 
  /*
   sets the ASYNC_TO_CHIP0 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_TO_CHIP0 = 70U,
  /*
   sets the ASYNC_TO_CHIP1 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_TO_CHIP1 = 71U,
  /*
   sets the ASYNC_TO_CHIP2 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_TO_CHIP2 = 72U,
  /*
   sets the ASYNC_TO_CHIP3 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_TO_CHIP3 = 73U,
  /*
   sets the ASYNC_TO_CHIP4 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_TO_CHIP4 = 74U,
  /*
   sets the ASYNC_TO_CHIP5 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_TO_CHIP5 = 75U,
  /*
   sets the ASYNC_TO_CHIP6 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_TO_CHIP6 = 76U,
  /*
   sets the ASYNC_TO_CHIP7 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_TO_CHIP7 = 77U,
  /*
   sets the ASYNC_FROM_CHIP0 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_FROM_CHIP0 = 80U,
  /*
   sets the ASYNC_FROM_CHIP1 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_FROM_CHIP1 = 81U,
  /*
   sets the ASYNC_FROM_CHIP2 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_FROM_CHIP2 = 82U,
  /*
   sets the ASYNC_FROM_CHIP3 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_FROM_CHIP3 = 83U, 
  /*
   sets the ASYNC_FROM_CHIP4 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_FROM_CHIP4 = 84U,
  /*
   sets the ASYNC_FROM_CHIP5 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_FROM_CHIP5 = 85U, 
  /*
   sets the ASYNC_FROM_CHIP6 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_FROM_CHIP6 = 86U,
  /*
   sets the ASYNC_FROM_CHIP7 configuration
   uses config
    - config/sub header is the config state to be applied the aer interface
    - value is used according to the config sub header
  */
  IN_CONF_ASYNC_FROM_CHIP7 = 87U, 

  /*
   chip config to free up main headers
  */
  IN_CONF_UC = 99U, 

  /*
    in mapper key switches into sequence transmission mode,
    the next packet is considered the key all subsequent packages 
    are considdered values until IN_MAPPER_END.
  */
  IN_MAPPER_KEY = 190U, 
  /*
    In mapper end switches back to normal packet exec mode.
  */
  IN_MAPPER_END = 191U, 

  /*
     this packet is used to reset the full uC
     uses data
      - value is ignored
  */
  IN_RESET = 254U,
  /*
     This packet is used to align the communication protocol between the PC and the uC
     it is send by the PC and the uC will respond with the same sequence:
     the sequence is 9 bytes of 255U followed by 1 byte of 0U
     the 9 bytes are needed so that one of them is interpreted as the header
  */
  IN_ALIGN_COMMUNICATION_PROTOCOL = 255U,

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
     this packet is used to report how many free spots there are in the instruction ring buffer
  */
  OUT_FREE_INSTRUCTION_SPOTS = 101U,
  /*
   After a input pin change below threshold this records the change
   uses pin
    - exec_time the time the change occured
    - pin the pin id
    - value the new state (value is manatory)
  */
  OUT_PIN_LOW = 110U,
    /*
   After a input pin change above threshold this records the change
   uses pin
    - exec_time the time the change occured
    - pin the pin id
    - value the new state (value is manatory)
  */
  OUT_PIN_HIGH = 111U,
  /*
   responce to the IN_SPI0_32 packet
   uses data32
    - exec_time the current run time 
    - value the 32bit word that was read on the SPI
  */
  OUT_SPI0 = 120U,
  /*
   responce to the IN_SPI1_32 packet
   uses data32
    - exec_time the current run time 
    - value the 32bit word that was read on the SPI
  */
  OUT_SPI1 = 121U,
  /*
   responce to the IN_SPI2_32 packet
   uses data32
    - exec_time the current run time 
    - value the 32bit word that was read on the SPI
  */
  OUT_SPI2 = 122U,

  /*
   responce to the IN_I2C0 packet
   uses data32
    - exec_time the current run time 
    - value the 8/16bit word that was read on the I2C
  */
  OUT_I2C0 = 125U,
  /*
   responce to the IN_I2C1 packet
   uses data32
    - exec_time the current run time 
    - value the 8/16bit word that was read on the I2C
  */
  OUT_I2C1 = 126U,
  /*
   responce to the IN_I2C2 packet
   uses data32
    - exec_time the current run time 
    - value the 8/16bit word that was read on the I2C
  */
  OUT_I2C2 = 127U,
  /*
   a event was reseved on ASYNC_FROM_CHIP0
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_ASYNC_FROM_CHIP0 = 130U,
  /*
   a event was reseved on ASYNC_FROM_CHIP1
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_ASYNC_FROM_CHIP1 = 131U,
  /*
   a event was reseved on ASYNC_FROM_CHIP2
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_ASYNC_FROM_CHIP2 = 132U,
  /*
   a event was reseved on ASYNC_FROM_CHIP3
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_ASYNC_FROM_CHIP3 = 133U,
  /*
   a event was reseved on ASYNC_FROM_CHIP4
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_ASYNC_FROM_CHIP4 = 134U,
  /*
   a event was reseved on ASYNC_FROM_CHIP5
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_ASYNC_FROM_CHIP5 = 135U,
  /*
   a event was reseved on ASYNC_FROM_CHIP6
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_ASYNC_FROM_CHIP6 = 136U,
  /*
   a event was reseved on ASYNC_FROM_CHIP7
   uses data
    - exec_time the current run time 
    - value the 0-32bit word that was read on the AER
  */
  OUT_ASYNC_FROM_CHIP7 = 137U,
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
  OUT_ERROR_ASYNC_HS_TIMEOUT = 209U,
  /*
   this error is thrown when a peripheral interface is not ready
   For example: the I2C for the MCP23017 is not ready
   uses 
  */
  OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY = 210U,
  /*
   the configuration id you send is out of bounds of the available uC resources
   uses error_package
    - id is the wrong id
  */
  OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS = 211U,
  /*
   the data you send is larger the the configured bit/byte width of the interface
  uses error_package
    - the wrong data
  */
  OUT_ERROR_DATA_OUT_OF_BOUNDS = 212U,
  /*
   the data collection is taking up to much time (to many requests)
   the uC does not have time to ship the data to the PC.
   from now on whenever this happens the uC will pause data collection
   for a moment and transmit ~ 10 pakages to the PC before it resumes 
   data collection
  */
  OUT_WARNING_DATA_COLLECTION_SQUEUED = 213U,

  /*
     responce to alignment request, as the first communication is an alignment
     confirms the connection with the uC.
     also sends the firmware version - to see if the correct version is running
     uses data_i2c
      - exec_time the current run time 
      - device address - major version
      - register address - minor version
      - value_ms the MS 8bit of patch version
      - value_ls the LS 8bit of patch version
  */
  OUT_ALIGN_SUCCESS_VERSION = 253U,
  
};


/*
  set an interface to active, after activation the pins/width cant be changed anymore
  works for spi and aer, activation will fail if pins are already used for other interfaces
  @TODO deactivation not implemented yet sould be new header for compatebility
  uses config
  - value is ignored
*/  
enum confPacketHeader : uint8_t {
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
   interface_order
    - value LSFIRST = 0 and MSFIRST = 1 - default is 0
  */
  CONF_BYTE_ORDER = 74U,
  /*
   interface speed class se interface doc
  */
  CONF_SPEED_CLASS = 75U,
  /*
   interface_type
    - value type id (see interface doc) - default is 0
  */
  CONF_TYPE = 76U,


  // ----------------------------------- Added for Recurrency mapping -----------------------------------
  SAVE_MAPPING_TABLE_MIN = 100, 
  // Should store an entry of the mapping table to the AO microcontroller.
  // Any value between 101 and 180 indicates the mapping_tabel (100) and INPUT neuron address (1-80) 
  
  SAVE_MAPPING_TABLE_MAX = 180, 
  // Should store an entry of the mapping table to the AO microcontroller.
  // Any value between 101 and 180 indicates the mapping_tabel (100) and INPUT neuron address (1-80) 



  /*
    indication of no sub category
  */
  CONF_NONE = 253U,
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
the data_i2c_t packet format: 9 bytes
*/
#pragma pack(push,1)
struct data_i2c_t{
  uint8_t header;
  uint32_t exec_time;
  uint8_t component_address;
  uint8_t register_address;
  uint8_t value_ms;
  uint8_t value_ls;
};
#pragma pack(pop)
/*
the config packet format: 9 bytes
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
the error packet format: 7 bytes
*/
#pragma pack(push,1)
struct error_t{
  uint8_t header;
  uint8_t org_header;
  uint32_t value;
  uint8_t sub_header;
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
  data_i2c_t data_i2c;
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