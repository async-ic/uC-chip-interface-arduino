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
#ifndef INTERVALTIMER_H
#define INTERVALTIMER_H
#define INTERVALTIMER_MAX_TIMERS 2
#include <Arduino.h>

typedef void (*callfunction_t) ();

/**
This is a limited reimplmentaion of the teensy IntervalTimer API for the SAMD21 processors
on the teensy the build in function is used

the porting was needed as no libraries with usec could be found (by limited serching) for the SAMD21 uCs
it can only handle 2 timers, but could be extended to TC3 and so on
it can only handle periods up to 21844 usec (can be easily extended on demand)
it is hardcoded to a CPU ferquency of 48MHz (could be made flexible on demand)
it uses TC4 and TC5 for interrupts

Limitation: it seams to not handle nested interrupts.
*/
class IntervalTimer {
    public:
        IntervalTimer() {
            counter_id = INTERVALTIMER_MAX_TIMERS;
            last_period = 100U;
            last_priority = 3U;
        }
        ~IntervalTimer() {end();}
        
        /**
         * @brief setup and start the timer
         * 
         * attaches the IntervalTimer object with a physical interupt timer, and starts it.
         * 
         * @param callback function to be called
         * @param usec period of the timer
         * @return true if timer successfully started, false if no physical interrupt timer is available
        */
        bool begin(callfunction_t callback, uint32_t usec) volatile;
        /**
         * @brief update the period of the timer.
         * restart the timer to the new period, if timer was running
        */
        void update(uint32_t usec) volatile;
        /**
         * @brief stop the timer
         * releases the physical interrupt timer
        */
        void end() volatile;
        /**
         * @brief set the interrupt priority of the timer
         * only 0-3 availible on SAMD21
         * calls update if timer is running
         * @param priority 0-3
        */
        void priority(uint8_t priority) volatile;

        /**
         * @brief the internal storage for the interrupt service routine for hardware timer TC5
        */
        static callfunction_t _TC5_callfunc;

        /**
         * @brief the internal storage for the interrupt service routine for hardware timer TC4
        */
        static callfunction_t _TC4_callfunc;

    private:
        //static TcCount16* TC[INTERVALTIMER_MAX_TIMERS];
        /**
         * @brief internal variable for keeping track of the avalibility of the timers
        */
        static bool counter_active[INTERVALTIMER_MAX_TIMERS];

        /**
         * @brief internal variable for keeping track of the hardware timer id
        */
        uint8_t counter_id;

        /**
         * @brief internal variable for keeping track of the period of the timer
        */
        uint32_t last_period;

        /**
         * @brief internal variable for keeping track of the priority of the timer
        */
        uint8_t last_priority;

        bool init_ran;

};
/* 
Interrupt Service Routine for TC4
for use by class
*/
extern void TC4_Handler();
/* 
Interrupt Service Routine for TC5
for use by class
*/
extern void TC5_Handler();

#endif
#endif