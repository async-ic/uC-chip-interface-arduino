/*
    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
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

#include "datatypes.h"
#include "core_ring_buffer.h"

// function to copy packet content from volotile to not volotile
packet_t copy_packet(volatile packet_t* in){
      packet_t out;
      uint8_t i = 0;
      for (i = 0; i < sizeof(packet_t); i++) out.bytes[i] = in->bytes[i]; 
      return out;
}

// function to copy packet content from not volotile to volotile
void copy_packet(packet_t* in, volatile packet_t* out){
      uint8_t i = 0;
      for (i = 0; i < sizeof(packet_t); i++) out->bytes[i] = in->bytes[i]; 
}

