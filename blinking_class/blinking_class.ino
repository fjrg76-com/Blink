/*Copyright (C) 
 * 2022 - fjrg76 at hotmail dot com
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
 */

// Blink class doesn't depend on the Arduino board support package, but we need
// this header in order to get the digitalWrite() function's signature for the 
// binding:
#include <Arduino.h>

#include "Blink.hpp"


#define SYSTEM_TICK 10 // in milliseconds
#define MS_TO_TICKS( x ) ( (x) / ( SYSTEM_TICK ) )


enum eOutputs{ GREEN = 2, RED = 3, LED13 = 13 };


// Arduino Nano Every board support package uses a different signature, so we 
// need to use an adapter:
void my_digitalWrite( uint8_t pin, uint8_t pin_state )
{
   digitalWrite( pin, pin_state );
}


using fjrg76::Blink;
using fjrg76::Output_fn;


// Hardware objects are usually placed at global scope (avoid globals in any 
// other case):
Blink led13;
Blink buzzer;
Blink relay;


// bind the hardware to the class:
Output_fn Blink::write = digitalWrite;


void setup() 
{
   pinMode( LED13, OUTPUT ); // on-board LED (active high)
   pinMode( GREEN, OUTPUT ); // simulates a buzzer (active low)
   pinMode( RED, OUTPUT );   // simulates a relay (active low)

   digitalWrite( LED13, LOW );
   digitalWrite( GREEN, HIGH );
   digitalWrite( RED, HIGH );


   led13.begin( LED13 );
   // ACTIVE_HIGH by default

   buzzer.begin( GREEN, Blink::ePolarity::ACTIVE_LOW );
   relay.begin( RED, Blink::ePolarity::ACTIVE_LOW );


   // led13 blinks forever:
   led13.set( Blink::eMode::FOREVER, MS_TO_TICKS( 100 ), MS_TO_TICKS( 900 ) );
   led13.start();


   buzzer.set( Blink::eMode::REPETITIVE, MS_TO_TICKS( 100 ), MS_TO_TICKS( 100 ), 10 );
   // buzzer will cycle 10 times every 5 seconds. You can change this behavior anytime.

   relay.set( Blink::eMode::ONCE, MS_TO_TICKS( 2000 ) );
   // relay will stay ON for two seconds every 7 seconds, then it will turn off. You can 
   // change this behavior anytime.
}

void loop()
{
   static unsigned long last_time = 0;
   unsigned long now = millis();
   if( ( now - last_time >= ( SYSTEM_TICK ) ) )
   {
      last_time = now;

      // state machines MUST be called on every system tick:
      led13.state_machine();
      buzzer.state_machine();
      relay.state_machine();


      // One second time base:
      static uint16_t seconds = MS_TO_TICKS( 1000 );
      --seconds;
      if( seconds == 0 )
      {
         seconds = MS_TO_TICKS( 1000 );


         // do something every 5 seconds:
         static uint8_t counter0 = 5; 
         --counter0;
         if( counter0 == 0 )
         {
            counter0 = 5;

            buzzer.start();
         }


         // do something every 7 seconds:
         static uint8_t counter1 = 7; 
         --counter1;
         if( counter1 == 0 )
         {
            counter1 = 7;

            relay.start();
         }
      }
   }


   // Regular Arduino's code:
   
}
