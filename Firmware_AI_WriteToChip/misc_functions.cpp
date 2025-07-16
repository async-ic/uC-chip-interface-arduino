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

#include "misc_functions.h"

/**

  kinda accurate for CPU ferquencies from 400MHz-2GHz, 
  below the resolution becomes more coarse as it min 8 instructions for the loop.
  eg. for a 48MHz processor delay20ns(1) to delay20ns(7) in ~210ns,
  delay20ns(8) to delay20ns(15) in ~420ns, and so on

*/

void delay20ns( uint8_t clocks )
{
    #if (F_CPU<=50000000)
    clocks = 1 + clocks >> 3;
    #elif (F_CPU<=100000000)
    clocks = 1 + clocks >> 2;
    #elif (F_CPU<=200000000)
    clocks = 1 + clocks >> 1;
    #endif
  do
  {
    __asm( "nop" );
    #if (F_CPU>400000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>450000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>500000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>550000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>600000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>650000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>700000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>750000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>800000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>850000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>900000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>950000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>1000000000)
    __asm( "nop" );
    #endif
    #if (F_CPU>1100000000)
    __asm( "nop" );
    __asm( "nop" );
    #endif
    #if (F_CPU>1200000000)
    __asm( "nop" );
    __asm( "nop" );
    #endif
    #if (F_CPU>1300000000)
    __asm( "nop" );
    __asm( "nop" );
    #endif
    #if (F_CPU>1400000000)
    __asm( "nop" );
    __asm( "nop" );
    #endif
    #if (F_CPU>1500000000)
    __asm( "nop" );
    __asm( "nop" );
    #endif
    #if (F_CPU>1600000000)
    __asm( "nop" );
    __asm( "nop" );
    #endif
    #if (F_CPU>1700000000)
    __asm( "nop" );
    __asm( "nop" );
    #endif
    #if (F_CPU>1800000000)
    __asm( "nop" );
    __asm( "nop" );
    #endif
    #if (F_CPU>1900000000)
    __asm( "nop" );
    __asm( "nop" );
    #endif
    #if (F_CPU>2000000000)
    __asm( "nop" );
    __asm( "nop" );
    #endif
    #if (F_CPU>2100000000)
     #error what uC are you running? function delay20ns only works till 2GHz
    #endif
  }
  while ( --clocks );
}



