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
#include "sensors.h"

#define SENSOR_HISTORYSIS      10

uint8_t sensor_mask;
uint8_t sensors_state;
uint8_t sensors_count[7];

void sensors_init(void)
{
    uint8_t i;

    // Make sure pullups are enabled.
    MCUCR &= ~(1<<PUD);

    // Enable PD2 - PD6 as inputs.
    DDRD &= ~((1<<DDD2) | (1<<DDD3) | (1<<DDD4) | (1<<DDD5) | (1<<DDD6));

    // Enable PD2-PD6 pull-up resistors.
    PORTD |= (1<<PD2) | (1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6);

    // Initialize the sensors state.
    sensors_state = 0x00;

    // Initialize the sensors counters.
    for (i = 0; i < 7; ++i)
    {
        // Initialize the sensor count.
        sensors_count[i] = 0;
    }
}


void sensors_update(void)
// Update the sensors state.
{
    uint8_t i;

    // Get the current sensor input status.
    uint8_t sensors_input = PIND;

    // Adjust the sensor input so the sensor bit is high when and obstruction or edge is detected.
    sensors_input = sensors_input & ((1<<PIND6) | (1<<PIND5) | (1<<PIND4) | (1<<PIND3) | (1<<PIND2));

    // Loop over each sensor input bit and update the corresponding sensor state.
    for (i = 0; i < 7; ++i)
    {
        // Is this sensor input bit active?
        if (sensors_input & (1<<i))
        {
            // Increment the button count if count is less than historisis.
            // XXX if (sensors_count[i] < SENSOR_HISTORYSIS) ++sensors_count[i];
            if (sensors_count[i] < SENSOR_HISTORYSIS) sensors_count[i] = SENSOR_HISTORYSIS;

            // Mark the button as pressed if count is at historisis.
            if (sensors_count[i] == SENSOR_HISTORYSIS) sensors_state |= (1<<i);
        }
        else
        {
            // Decrement the button if count is non-zero.
            if (sensors_count[i] > 0) --sensors_count[i];

            // Mark the button as not pressed if count is at zero.
            if (sensors_count[i] == 0) sensors_state &= ~(1<<i);
        }
    }

}



uint8_t sensors_get(void)
// Return the sensors state.
{
    return sensors_state;
}



uint8_t sensors_triggered(uint8_t sensors_mask)
// Return the sensors triggered excluding those in the mask.
{
    return sensors_state & ~sensors_mask;
}

