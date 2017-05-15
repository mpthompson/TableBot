/*
    Copyright (c) 2006 Michael P. Thompson <mpthompson@gmail.com>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id:$
*/

#include <avr/io.h>
#include "leds.h"

void leds_init(void)
{
    // Enable PB3 and PB4 as outputs.
    DDRB |= ((1<<DDB4) | (1<<DDB3));

    // Turn off the green LED.
    PORTB &= ~(1<<PB3);

    // Turn off the yellow LED.
    PORTB &= ~(1<<PB1);
}


void leds_green_on(void)
{
    // Turn on the green LED.
    PORTB |= (1<<PB3);
}


void leds_green_off(void)
{
    // Turn off the green LED.
    PORTB &= ~(1<<PB3);
}


void leds_yellow_on(void)
{
    // Turn on the yellow LED.
    PORTB |= (1<<PB4);
}


void leds_yellow_off(void)
{
    // Turn off the yellow LED.
    PORTB &= ~(1<<PB4);
}


