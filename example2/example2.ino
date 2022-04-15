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

enum{ LED13=13};


void my_digitalWrite( uint8_t x, uint8_t y )
{
   digitalWrite( x, y );
}


using fjrg76::Blink;
using fjrg76::Output_fn;

// bind the hardware to the class:
Output_fn Blink::write = my_digitalWrite;


Blink pulses;

void pulse_ISR()
{
   pulses.state_machine();
}



void setup() 
{
   pinMode( 2, INPUT_PULLUP );
   attachInterrupt( digitalPinToInterrupt( 2 ), pulse_ISR, FALLING );

   pinMode( LED13, OUTPUT );
   pulses.begin( LED13, Blink::ePolarity::ACTIVE_HIGH );
   pulses.set( Blink::eMode::ONCE, 5 );

   pinMode( 3, INPUT );
   // botón externo: final de carrera, inicio, etc...

}

void loop()
{
   if( digitalRead( 3 ) == 0 and not pulses.is_running() )
   {
      while( digitalRead( 3 ) == 0 ) ;
      // esperamos a que el botón se deje de presionar

      pulses.start();
      // activa al actuador e inicia la cuenta de pulsos
   }
}
