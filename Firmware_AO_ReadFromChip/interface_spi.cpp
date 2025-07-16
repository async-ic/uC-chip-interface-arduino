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

#include "interface_spi.h"
#include "uc_boards.h"

volatile bool Interface_spi::active[SPI_NUMBER_OF_INTERFACES] = {};
volatile uint8_t Interface_spi::width[SPI_NUMBER_OF_INTERFACES]= {};
volatile uint32_t Interface_spi::frequency[SPI_NUMBER_OF_INTERFACES]= {};
volatile uint8_t Interface_spi::mode[SPI_NUMBER_OF_INTERFACES]= {};
volatile bool Interface_spi::byte_order[SPI_NUMBER_OF_INTERFACES]= {};
volatile Interface_spi* Interface_spi::inst[SPI_NUMBER_OF_INTERFACES]= {};

#pragma pack(push,1)
union bytepack_t{
  uint8_t bytes[4];
  uint32_t data;
};
#pragma pack(pop)

void Interface_spi::configure(uint8_t id, uint8_t config_option, uint8_t data){

  if (id >= SPI_NUMBER_OF_INTERFACES) {
    error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_SPI0,id,CONF_ACTIVE);
    return;
  }
  uint8_t interface;
  switch (id){
    default:
    case 0: interface = IN_CONF_SPI0; break;
    case 1: interface = IN_CONF_SPI1; break;
    case 2: interface = IN_CONF_SPI2; break;
  }
  if (Interface_spi::active[id] == 1) error_message(OUT_ERROR_INTERFACE_ALREADY_ACTIVE,interface,config_option);
  else {
    switch (config_option){
      case CONF_ACTIVE:
        Interface_spi::inst[id] = new Interface_spi(id,
                                    Interface_spi::frequency[id],
                                    Interface_spi::width[id],
                                    Interface_spi::mode[id],
                                    Interface_spi::byte_order[id]);
        send_config(interface,config_option,1);
        return;
      case CONF_WIDTH:
        if (data > 4 || data <=0) {
          error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,interface,data,config_option);
          return;
        }
        Interface_spi::width[id] = data;
        send_config(interface,config_option,data);
        return;
      case CONF_BYTE_ORDER:
        if (data > 0){
          Interface_spi::byte_order[id] = true;
          send_config(interface,config_option,1);
        }
        else{
          Interface_spi::byte_order[id] = false;
          send_config(interface,config_option,0);
        }
        return;
      case CONF_SPEED_CLASS:
        switch (data){
          case 0:
              Interface_spi::frequency[id] = 10000U;
              break;
          case 1:
              Interface_spi::frequency[id] = 50000U;
              break;
          default:
          case 2:
              Interface_spi::frequency[id] = 100000U;
              send_config(interface,config_option,2);
              return;
          case 3:
              Interface_spi::frequency[id] = 500000U;
              break;
          case 4:
              Interface_spi::frequency[id] = 1000000U;
              break;
          case 5:
              Interface_spi::frequency[id] = 2000000U;
              break;
          case 6:
              Interface_spi::frequency[id] = 4000000U;
              break;
          case 7:
              Interface_spi::frequency[id] = 8000000U;
              break;
          case 8:
              Interface_spi::frequency[id] = 12000000U;
              break;
        }
        send_config(interface,config_option,data);
        return;

      case CONF_TYPE:
        switch (data){
          default:
          case 0:
              Interface_spi::mode[id] = SPI_MODE0;
              send_config(interface,config_option,0);
              return;
          case 1:
              Interface_spi::mode[id] = SPI_MODE1;
              break;
          case 2:
              Interface_spi::mode[id] = SPI_MODE2;
              break;
          case 3:
              Interface_spi::mode[id] = SPI_MODE3;
              break;
        }
        send_config(interface,config_option,data);
        return;

      default:
        error_message(OUT_ERROR_UNKNOWN_CONFIGURATION,config_option,data);
        return;
    }
  }
}     

Interface_spi::Interface_spi(uint8_t id, uint32_t frequency, uint8_t width, uint8_t mode, bool byte_order){
  if (id >= SPI_NUMBER_OF_INTERFACES) {
    error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_SPI0,id,CONF_ACTIVE);
    return;
  }
  uint8_t interface;
  switch (id){
    default:
    case 0: interface = IN_CONF_SPI0; break; 
    case 1: interface = IN_CONF_SPI1; break;
    case 2: interface = IN_CONF_SPI2; break;
  }
  _id=id;
  _frequency = frequency;
  if (width == 0) {
    _width = 1;
    send_config(interface,CONF_WIDTH,1);
  }
  else _width=width;
  if (mode != SPI_MODE0 ||mode != SPI_MODE1 ||mode != SPI_MODE2 ||mode != SPI_MODE3 ) {
    _mode = SPI_MODE0;
    send_config(interface,CONF_TYPE,0);
  }
  else _mode=mode;
  _byte_order=byte_order;
  switch (_id){
    default:
    case 0:
      if (reserve_input_pin(SPI_SCK_PORT) && reserve_output_pin(SPI_CIPO_PORT) && reserve_output_pin(SPI_COPI_PORT)){
        _spi = &SPI;
        _spi->begin();
        Interface_spi::active[0] = 1;
        // send confimation
        send_config(interface,CONF_ACTIVE,1);                  
      }
      break; 
    #if (SPI_NUMBER_OF_INTERFACES > 1)              
    case 1: 
      if (reserve_input_pin(SPI_SCK1_PORT) && reserve_output_pin(SPI_CIPO1_PORT) && reserve_output_pin(SPI_COPI1_PORT)){
        _spi = &SPI1;
        _spi->begin();
        Interface_spi::active[1] = 1;
        // send confimation
        send_config(interface,CONF_ACTIVE,1);                    
      }
      break;
    #endif
    #if (SPI_NUMBER_OF_INTERFACES > 2)  
    case 2: 
      if (reserve_input_pin(SPI_SCK2_PORT) && reserve_output_pin(SPI_CIPO2_PORT) && reserve_output_pin(SPI_COPI2_PORT)){
        _spi = &SPI2;
        _spi->begin();
        Interface_spi::active[2] = 1;
        // send confimation
        send_config(interface,CONF_ACTIVE,1);                      
      }
      break;
    #endif
  }
}  
void Interface_spi::send_packet(uint8_t id, uint32_t data){
  if (id >= SPI_NUMBER_OF_INTERFACES) {
    error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,IN_SPI0,id,CONF_ACTIVE);
    return;
  }
  else if (!Interface_spi::active[id]){
    switch (id){
        default:
        case 0: error_message(OUT_ERROR_INTERFACE_NOT_ACTIVE,IN_SPI0,id); break;
        case 1: error_message(OUT_ERROR_INTERFACE_NOT_ACTIVE,IN_SPI1,id); break;
        case 2: error_message(OUT_ERROR_INTERFACE_NOT_ACTIVE,IN_SPI2,id); break;
      }
  }
  else{
    Interface_spi::inst[id]->write(data);
  }
}

void Interface_spi::write(uint32_t data) volatile {
  bytepack_t buffer;
  uint8_t interface_in, interface_out;
  //@TODO class var
  switch (_id){
    default:
    case 0: interface_in = IN_SPI0; interface_out = OUT_SPI0; break;
    case 1: interface_in = IN_SPI1; interface_out = OUT_SPI1; break;
    case 2: interface_in = IN_SPI2; interface_out = OUT_SPI2; break;
  }
  if (data >= pow(2,(8*Interface_spi::width[_id]))){
    error_message(OUT_ERROR_DATA_OUT_OF_BOUNDS,interface_in,data,CONF_WIDTH);
    return;
  }
  buffer.data = data;
  _spi->beginTransaction(SPISettings(Interface_spi::frequency[_id],
                                          Interface_spi::byte_order[_id] ? MSBFIRST : LSBFIRST,
                                         (Interface_spi::mode[_id] == 3) ? SPI_MODE3 : 
                                              ((Interface_spi::mode[_id] == 2) ? SPI_MODE2 : 
                                              ((Interface_spi::mode[_id] == 1) ? SPI_MODE1 : SPI_MODE0))));
  _spi->transfer(buffer.bytes,Interface_spi::width[_id]);
  _spi->endTransaction();

  send_data32(interface_in,data,true);
  send_data32(interface_out,buffer.data);
}
