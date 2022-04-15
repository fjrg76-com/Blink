/*Copyright (C)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * 2022 - fjrg76 dot com (c)
 */

#include "Blink.hpp"

namespace fjrg76
{
   /**
    * @brief Creates an empty blinking object.
    */
   Blink::Blink()
   {
      // nothing
   }

   /**
    * @brief Initializes this blinking object.
    *
    * @param pin Binds this object to the microcontroller's pin.
    * @param polarity Sets the pin's active state. @see ePolarity.
    */
   void Blink::begin( uint8_t pin, ePolarity polarity )
   {
      this->pin = pin;
      this->polarity = polarity;

      write( this->pin, static_cast<uint8_t>(this->polarity) ^ PIN_LOW );
   }

   /**
    * @brief Configures this object.
    *
    * @param mode Sets the operational pin's mode. @see eMode.
    * @param ticks_on The number of ticks that the pin will stay in its active state (ON).
    * @param ticks_off The number of ticks that the pin will stay in its inactive state (OFF).
    * @param times The number of cycles that the pin will repeat. This parameter only has meaning when the pin's mode has been set as eMode::REPETITIVE, and it's ignored in the other modes.
    */
   void Blink::set( eMode mode, uint16_t ticks_on, uint16_t ticks_off, uint16_t times )
   {
      this->mode         = mode;
      this->ticks_onMOD  = ticks_on;
      this->ticks_offMOD = ticks_off;
      this->timesMOD     = times;
   }

   /**
    * @brief Starts the cycle.
    */
   void Blink::start()
   {
      this->running = false;

      this->ticks = this->ticks_onMOD;

      if( this->mode == eMode::REPETITIVE )
      {
         this->times = this->timesMOD;
      }

      this->state = 0;
      this->running = true;

      write( this->pin, static_cast<uint8_t>(this->polarity) ^ PIN_HIGH );
   }

   /**
    * @brief Stops the cycle.
    */
   void Blink::stop()
   {
      this->running = false;
   }

   /**
    * @brief The core of the system. This method must be called on a regular basis,
    * better with periodic and constant time intervals.
    *
    * In a non-Arduino systems you can place it in the timer ISR; in Arduino
    * systems use the millis() function and avoid blocking calls (like delay()).
    */
   void Blink::state_machine()
   {
      if( this->running )
      {
         switch( this->state )
         {
            case 0:
               --this->ticks;
               if( this->ticks == 0 )
               {
                  write( this->pin, static_cast<uint8_t>(this->polarity) ^ PIN_LOW );

                  if( this->mode == eMode::REPETITIVE or this->mode == eMode::FOREVER )
                  {
                     this->ticks = this->ticks_offMOD;
                     this->state = 1;
                  }
                  else
                  {
                     this->running = false;
                  }
               }
               break;

            case 1:
               --this->ticks;
               if( this->ticks == 0 )
               {
                  if( this->mode == eMode::REPETITIVE )
                  {
                     --this->times;
                     if( this->times == 0 )
                     {
                        this->running = false;
                     }
                  }

                  if( this->running )
                  {
                     this->state = 0;
                     this->ticks = this->ticks_onMOD;

                     write( this->pin, static_cast<uint8_t>(this->polarity) ^ PIN_HIGH );
                  }
               }
               break;

         } // switch state
      } // if this->running
   }

   /**
    * @brief Indicates whether the pin is running.
    *
    * @return true if the pin is running; false otherwise.
    */
   bool Blink::is_running()
   {
      return this->running;
   }

   /**
    * @brief The pin is always ON regardless of its configuration (useful when
    * the pin must remain ON all the time).
    */
   void Blink::always_on()
   {
      this->running = false;
      write( this->pin, static_cast<uint8_t>(this->polarity) ^ PIN_HIGH );
   }

   /**
    * @brief The pin is always OFF regardless of its configuration (useful when
    * the pin must remain OFF all the time).
    */
   void Blink::always_off()
   {
      this->running = false;
      write( this->pin, static_cast<uint8_t>(this->polarity) ^ PIN_LOW );
   }
}
