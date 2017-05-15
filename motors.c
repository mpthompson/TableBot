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
#include "motors.h"

void motors_init(void)
{
    // Make sure the motor A and motor B are disabled.
    PORTB |= (PB0 | PB5);

    // Lower the motor outputs.  These will be taken over by the clock below.
    PORTB &= ~(PB2 | PB1);

    // Enable PB0, PB1, PB2 and PB5 as outputs.
    DDRB |= ((1<<DDB5) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0));

    // Stop the timer counter.
    TCCR1B = 0;

    // Set the timer compare registers for a 50% duty cycle.
    TCNT1 = 0x00;
    OCR1A = MOTORS_IDLE_PWM;
    OCR1B = MOTORS_IDLE_PWM;

    // Enable timer 1A and timer 1B.
    TCCR1A = (1<<COM1A1) | (0<<COM1A0) |                    // Clear OC1A on compare match on up-count, set at top.
             (1<<COM1B1) | (1<<COM1B0) |                    // Set OC1B on compare match on up-count, clear at top.
             (0<<WGM11) | (1<<WGM10);                       // Fast PWM, 8-bit.

    // Set clock select bits to start timer.
    TCCR1B = (0<<ICNC1) | (0<<ICES1) |                      // Input on ICP1 disabled.
             (0<<WGM13) | (1<<WGM12) |                      // Fast PWM, 8-bit.
             (0<<CS12) | (0<<CS11) | (1<<CS10);             // Clk/1 - no prescaling.

    // Clear TCCR1C when operating in a PWM mode.
    TCCR1C &= ~(FOC1A | FOC1B);

    // Enable motor A and motor B.
    PORTB &= ~(PB0 | PB5);
}


void motors_a_pwm(int16_t pwm)
{
    int16_t pwm_output;

    // Sanity check for maximum and minimum values.
    if (pwm > MOTORS_MAX_PWM) pwm = MOTORS_MAX_PWM;
    if (pwm < MOTORS_MIN_PWM) pwm = MOTORS_MIN_PWM;

    // Determine the PWM output.
    pwm_output = MOTORS_IDLE_PWM + pwm;

    // Update the PWM value.
    OCR1A = (uint16_t) pwm_output;
}


void motors_b_pwm(int16_t pwm)
{
    int16_t pwm_output;

    // Sanity check for maximum and minimum values.
    if (pwm > MOTORS_MAX_PWM) pwm = MOTORS_MAX_PWM;
    if (pwm < MOTORS_MIN_PWM) pwm = MOTORS_MIN_PWM;

    // Determine the PWM output.
    pwm_output = MOTORS_IDLE_PWM + pwm;

    // Update the PWM value.
    OCR1B = (uint16_t) pwm_output;
}

