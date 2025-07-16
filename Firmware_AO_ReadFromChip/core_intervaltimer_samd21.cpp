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

#if defined(ARDUINO_ARCH_SAMD)
// hardcoded to 48MHz should check via F_CPU flag
#include "core_intervaltimer_samd21.h"
#include "core_ring_buffer.h"

callfunction_t IntervalTimer::_TC5_callfunc = NULL;
callfunction_t IntervalTimer::_TC4_callfunc = NULL;
//TcCount16* IntervalTimer::TC[INTERVALTIMER_MAX_TIMERS] = {(TcCount16*) &(TC4->COUNT32),(TcCount16*) &(TC5->COUNT32)};
bool IntervalTimer::counter_active[INTERVALTIMER_MAX_TIMERS] = {};


bool IntervalTimer::begin(callfunction_t callback, uint32_t usec) volatile {
    if (usec < 1431655765U)     last_period = usec;
    else {
        // bigger range can be implemented by choosing scaling factors, @TODO on demand
        // @todo replace source header with timer config once implemented for PWM/analog sampling
        error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,OUT_ERROR,usec);
        return false;
    }
    if (counter_id >= INTERVALTIMER_MAX_TIMERS){
        noInterrupts();
        counter_id = 0;
        while (counter_id < INTERVALTIMER_MAX_TIMERS && counter_active[counter_id]){
            counter_id++;
        }
        if (counter_id >= INTERVALTIMER_MAX_TIMERS){
            interrupts();
            return false;
        }
        counter_active[counter_id] = true;
        interrupts();
    }

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |                     // Enable GCLK0
                        GCLK_CLKCTRL_ID_TC4_TC5 |                // GCLK0 output to TC4, TC5
                        GCLK_CLKCTRL_GEN_GCLK0;                  // Select GCLK0 at CPU Speed


    while (GCLK->STATUS.bit.SYNCBUSY);  // Wait for sync
    
    uint32_t period = last_period*3;
    switch (counter_id){
      case 0:  
        TC4->COUNT32.CC[0].reg = period;    // Set the TC CC0 register to stop value
        while (TC4->COUNT32.STATUS.bit.SYNCBUSY); // Wait for sync
        // set ISR
        _TC4_callfunc = callback;
        NVIC_DisableIRQ(TC4_IRQn);
        NVIC_ClearPendingIRQ(TC4_IRQn);
        NVIC_SetPriority(TC4_IRQn, last_priority);    // Set Nested Vector Interrupt Controller priority 
        NVIC_EnableIRQ(TC4_IRQn);  
        TC4->COUNT32.INTENSET.bit.OVF = 1;         // Enable overflow interrupt
    
        TC4->COUNT32.CTRLA.reg |= TC_CTRLA_PRESCSYNC_PRESC |     // Reset timer on the next prescaler clock
                                            TC_CTRLA_WAVEGEN_MFRQ |         // Mode to match frequency (MFRQ) 
                                            TC_CTRLA_PRESCALER_DIV16 |      // Set prescaler to 16 , for 0.333us
                                            TC_CTRLA_MODE_COUNT32;          // Mode to 32-bit

        while (TC4->COUNT32.STATUS.bit.SYNCBUSY);                // Wait for sync

        TC4->COUNT32.CTRLA.bit.ENABLE = 1;                       // Enable
        while (TC4->COUNT32.STATUS.bit.SYNCBUSY);                // Wait for sync
        break;
      case 1:      
        TC5->COUNT32.CC[0].reg = period;    // Set the TC CC0 register to stop value

        while (TC5->COUNT32.STATUS.bit.SYNCBUSY);                   // Wait for sync

        _TC5_callfunc = callback;
        NVIC_DisableIRQ(TC5_IRQn);
        NVIC_ClearPendingIRQ(TC5_IRQn);
        NVIC_SetPriority(TC5_IRQn, last_priority);    // Set Nested Vector Interrupt Controller priority
        NVIC_EnableIRQ(TC5_IRQn); 

        TC5->COUNT32.INTENSET.bit.OVF = 1;         // Enable overflow interrupt

        TC5->COUNT32.CTRLA.reg |= TC_CTRLA_PRESCSYNC_PRESC |     // Reset timer on the next prescaler clock
                                            TC_CTRLA_WAVEGEN_MFRQ |         // Mode to match frequency (MFRQ) 
                                            TC_CTRLA_PRESCALER_DIV16 |      // Set prescaler to 16 , for 0.333us
                                            TC_CTRLA_MODE_COUNT32;          // Mode to 32-bit

        while (TC5->COUNT32.STATUS.bit.SYNCBUSY);                // Wait for sync

        TC5->COUNT32.CTRLA.bit.ENABLE = 1;                       // Enable
        while (TC5->COUNT32.STATUS.bit.SYNCBUSY);                // Wait for sync
        break;
    }
    return true;
}

void IntervalTimer::update(uint32_t usec) volatile{
    if (usec < 21845U)     last_period = usec;
    else {
        // bigger range can be implemented by choosing scaling factors, @TODO on demand
        // @todo replace source header with timer config once implemented for PWM/analog sampling
        error_message(OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS,OUT_ERROR,usec);
        return;
    }
    switch(counter_id){
        case 0:
            begin(_TC4_callfunc,last_period);
            break;
        case 1:
            begin(_TC5_callfunc,last_period);
            break;
        default:
            return;
    }
}
void IntervalTimer::end() volatile {
    if (counter_id < INTERVALTIMER_MAX_TIMERS){
      switch (counter_id){
        case 0:
          TC4->COUNT32.CTRLA.bit.ENABLE = 0;                       // Disable
          while (TC4->COUNT32.STATUS.bit.SYNCBUSY);                // Wait for sync
          counter_id=INTERVALTIMER_MAX_TIMERS;
          break;
        case 1:
          TC5->COUNT32.CTRLA.bit.ENABLE = 0;                       // Disable
          while (TC5->COUNT32.STATUS.bit.SYNCBUSY);                // Wait for sync
          counter_id=INTERVALTIMER_MAX_TIMERS;
          break;
      }

    }
}
void IntervalTimer::priority(uint8_t priority) volatile {
    if (priority > 3) {
      last_priority = priority;
      //NVIC_SetPriority(EIC_IRQn, 2);
    }
    else last_priority = priority;
    update(last_period);
}

void TC4_Handler()  
{    
    TC4->COUNT32.INTFLAG.bit.OVF = 1;             // Clear OVF flag
    if (IntervalTimer::_TC4_callfunc != NULL) IntervalTimer::_TC4_callfunc();  
  //if (TC4->COUNT32.INTFLAG.bit.OVF && TC4->COUNT32.INTENSET.bit.OVF) {        
    //TC4->COUNT32.INTFLAG.bit.OVF = 1;             // Clear OVF flag
  //}
}

void TC5_Handler()                                         
{ 
    TC5->COUNT32.INTFLAG.bit.OVF = 1;             // Clear OVF flag
    if (IntervalTimer::_TC5_callfunc != NULL) IntervalTimer::_TC5_callfunc();
  //if (TC5->COUNT32.INTFLAG.bit.OVF && TC5->COUNT32.INTENSET.bit.OVF){        
    //TC5->COUNT32.INTFLAG.bit.OVF = 1;             // Clear OVF flag
  //}
}
#endif