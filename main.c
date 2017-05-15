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
#include <avr/interrupt.h>
#include <string.h>
#include "fsm.h"
#include "leds.h"
#include "motors.h"
#include "timer.h"
#include "sensors.h"
#include "usart.h"

#define DISPLAY_WIDTH       176
#define DISPLAY_HEIGHT      144
#define DISPLAY_LEFT_SIDE   ((DISPLAY_WIDTH / 2) - 10)
#define DISPLAY_RIGHT_SIDE   ((DISPLAY_WIDTH / 2) + 10)

// This is the color index we are looking for.
#define BLOB_COLOR          0

// Blob tracking information.
static uint16_t blob_size;
static uint8_t blob_center_x;
static uint8_t blob_center_y;

// Camera packet information.
static char camera_ack[8];
static char camera_packet[64];
static uint8_t camera_packet_len = 0;
static uint8_t camera_packet_num = 0;

void motors_stop(void)
{
    // Stop the motors.
    motors_a_pwm(0);
    motors_b_pwm(0);
}


void motors_start(int16_t pwm)
{
    // Set the motor PWM values.
    motors_a_pwm(pwm);
    motors_b_pwm(pwm);
}


void motors_rotate(int16_t a_pwm, int16_t b_pwm)
{
    // Set the motor PWM values.
    motors_a_pwm(a_pwm);
    motors_b_pwm(b_pwm);
}


void motors_search(void)
{
    // Default is to keep going forward.
    int16_t a_pwm = 32;
    int16_t b_pwm = 32;

    // Determine adjustment factor.
    if (blob_size && (blob_center_x < DISPLAY_LEFT_SIDE))
    {
        // Turn left.
        b_pwm -= (DISPLAY_LEFT_SIDE - ((int16_t) blob_center_x));

        // Make sure it is not too much.
        if (b_pwm < -32) a_pwm = -32;
    }
    else if (blob_size && (blob_center_x > DISPLAY_RIGHT_SIDE))
    {
        // Turn right.
        a_pwm -= (((int16_t) blob_center_x) - DISPLAY_RIGHT_SIDE);

        // Make sure it is not too much.
        if (a_pwm < -32) a_pwm = -32;
    }

    // Set the motor PWM values.
    motors_a_pwm(a_pwm);
    motors_b_pwm(b_pwm);
}


void motors_turnaway(uint8_t obstruction)
{
    // Be default turn left.
    int16_t a_pwm = 32;
    int16_t b_pwm = -32;

    // Determine the direction to turn.
    if ((obstruction == (1<<SENSOR_GROUND_LEFT_FRONT)) ||
        (obstruction == ((1<<SENSOR_GROUND_LEFT_FRONT) | (1<<SENSOR_GROUND_FRONT))))
    {
        // Turn right.
        a_pwm = -32;
        b_pwm = 32;
    }
    else if ((obstruction == (1<<SENSOR_GROUND_RIGHT_FRONT)) ||
             (obstruction == ((1<<SENSOR_GROUND_RIGHT_FRONT) | (1<<SENSOR_GROUND_FRONT))))
    {
        // Turn left.
        a_pwm = 32;
        b_pwm = -32;
    }
    else if (obstruction == (1<<SENSOR_GROUND_RIGHT_REAR))
    {
        // Turn right.
        a_pwm = -32;
        b_pwm = 32;
    }
    else if (obstruction == (1<<SENSOR_GROUND_LEFT_REAR))
    {
        // Turn left.
        a_pwm = 32;
        b_pwm = -32;
    }

    // Set the motor PWM values.
    motors_a_pwm(a_pwm);
    motors_b_pwm(b_pwm);
}


void motors_backaway(uint8_t obstruction)
{
    int16_t a_pwm = 0;
    int16_t b_pwm = 0;

    // Is it just the front sensors?
    if ((obstruction & SENSORS_FORWARD) && !(obstruction & SENSORS_REARWARD))
    {
        // Reverse slowly.
        a_pwm = -32;
        b_pwm = -32;
    }
    else if ((obstruction & SENSORS_REARWARD) && !(obstruction & SENSORS_FORWARD))
    {
        // Forward slowly.
        a_pwm = 32;
        b_pwm = 32;
    }

    // Set the motor PWM values.
    motors_a_pwm(a_pwm);
    motors_b_pwm(b_pwm);
}


int16_t tablebot_fsm()
// Implements TableBot finite state machine.
{
    static uint8_t obstruction = 0;

    FSM_BEGIN(SEARCH)

        FSM_STATE_BEGIN(SEARCH)

            // Set motors to go forward.
            motors_start(32);

            // Configure timer to wait a random amount of time.
            timer_wait_set(0, 50 + ((timer_random() & 0x07) << 4));

            fsm_checkpoint();

            // Keep an eye out for the blob.
            fsm_change_state(blob_size, PUSH);

            // Back away from obstructions.
            fsm_change_state(sensors_triggered(0), BACKAWAY);

            // If timer expires then rotate to look for blob.
            fsm_change_state(timer_wait_done(0), ROTATE);

        FSM_STATE_END

        FSM_STATE_BEGIN(PUSH)

            // Steer towards the block to push it.
            motors_search();

            // Back away from obstructions.
            fsm_change_state(sensors_triggered(0), BACKAWAY);

            // Rotate if we lost the blob.
            fsm_change_state(!blob_size, ROTATE);

        FSM_STATE_END

        FSM_STATE_BEGIN(ROTATE)

            // Stop the motors.
            motors_stop();

            // Set the timer to wait .5 seconds.
            timer_wait_set(0, 5);

            // Wait for timer to expire.
            fsm_wait_until(timer_wait_done(0));

            // Set time to wait for complete turn.
            timer_wait_set(0, 40);

            // Set the motors to rotate.
            motors_rotate(-32, 32);

            fsm_checkpoint();

            // Keep an eye out for the blob.
            fsm_change_state(blob_size, PUSH);

            // Back away from obstructions.
            fsm_change_state(sensors_triggered(0), BACKAWAY);

            // Wait to resume forward motion.
            fsm_change_state(timer_wait_done(0), SEARCH);

        FSM_STATE_END

        FSM_STATE_BEGIN(BACKAWAY)

            // Stop the motors.
            motors_stop();

            // Save the sensor data which indicates the location of the obstruction.
            obstruction = sensors_triggered(0);

            // Set the motor directions to stop.
            motors_backaway(obstruction);

            // Set the timer to wait 1 second.
            timer_wait_set(0, 10);

            fsm_checkpoint();

            // Keep lookout for new obstructions.
            fsm_change_state(sensors_triggered(obstruction), BACKAWAY);

            // Wait to back away from the obstruction.
            fsm_change_state(timer_wait_done(0), TURNAWAY);

        FSM_STATE_END

        FSM_STATE_BEGIN(TURNAWAY)

            // Stop the motors.
            motors_stop();

            // Are we clear of all obstructions?
            fsm_change_state(sensors_triggered(0), BACKAWAY);

            // Set the timer to wait .5 seconds.
            timer_wait_set(0, 5);

            // Wait for timer to expire.
            fsm_wait_until(timer_wait_done(0));

            // Set the motor direction to turn away.
            motors_turnaway(obstruction);

            // Set time to wait for turn to complete.
            timer_wait_set(0, 10);

            fsm_checkpoint();

            // Back away from obstructions.
            fsm_change_state(sensors_triggered(0), BACKAWAY);

            // Keep an eye out for the blob.
            fsm_change_state(blob_size, PUSH);

            // Wait to resume forward motion.
            fsm_change_state(timer_wait_done(0), SEARCH);
        FSM_STATE_END

    FSM_END
}


void camera_packet_process(void)
// Process a camera packet.  We essentially want to get the 
// largest blob from the packet and update the blob state.
{
    uint8_t i;
    uint8_t boxes;
    uint8_t box_index;
    uint8_t box_upper_left_x;
    uint8_t box_upper_left_y;
    uint8_t box_lower_right_x;
    uint8_t box_lower_right_y;
    uint16_t box_size;

    // Ignore packets that don't look valid.
    if (camera_packet[0] != 0x0A) return;
    if (camera_packet[camera_packet_len - 1] != 0xFF) return;

    // Reset the blob information.
    blob_size = 0;
    blob_center_x = 88;
    blob_center_y = 72;

    // Get the bounding box count.
    boxes = camera_packet[1];

    // Loop over each bounding box.
    for (i = 0; i < boxes; ++i)
    {
        // Fill in the box index.
        box_index = 2 + (i * 5);

        // Look for the blob color.
        if (camera_packet[box_index] != BLOB_COLOR) continue;

        // Get the bound box.
        box_upper_left_x = camera_packet[box_index + 1];
        box_upper_left_y = camera_packet[box_index + 2];
        box_lower_right_x = camera_packet[box_index + 3];
        box_lower_right_y = camera_packet[box_index + 4];
 
        // Get the box size as the taxi distance around half the box.
        box_size = (box_lower_right_x - box_upper_left_x);
        box_size += (box_lower_right_y - box_upper_left_y);

        // Should we update the blob information.
        if ((i == 0) || (box_size > blob_size))
        {
            // Fill the new blob information.
            blob_size = box_size;
            blob_center_x = (box_lower_right_x >> 1) + (box_upper_left_x >> 1);
            blob_center_y = (box_lower_right_y >> 1) + (box_upper_left_y >> 1);
        }
    }

    // Blink the tracking LED while tracking a blob.
    if (blob_size && (++camera_packet_num & 0x02)) leds_yellow_on(); else leds_yellow_off();
}


int16_t camera_fsm()
// Implements camera finite state machine.
{
    FSM_BEGIN(PING)

        FSM_STATE_BEGIN(PING)

            // Disable any tracking.
            usart_xmit_buffer("DT\r", 3);

            // Set the timer to wait 1 second.
            timer_wait_set(1, 10);

            // Pause until the timer is finished.
            fsm_wait_until(timer_wait_done(1));

            // Transmit the ping.
            usart_xmit_buffer("PG\r", 3);

            // Set the timer to wait 1 second.
            timer_wait_set(1, 10);

            fsm_checkpoint();

            // Check for ACK.
            fsm_change_state(usart_recv_buffer_has_eol('\r'), PING_ACK);

            // Restart if timer is done.
            fsm_change_state(timer_wait_done(1), PING);

        FSM_STATE_END

        FSM_STATE_BEGIN(PING_ACK)

            // Read the buffer of data.
            usart_recv_buffer(camera_ack, 8, '\r');

            // Start tracking if we got the ack.
            fsm_change_state (!strncmp(camera_ack, "ACK\r", 4), ENABLE_TRACKING);

            // Restart if not ack.
            fsm_change_state(1, PING);

        FSM_STATE_END

        FSM_STATE_BEGIN(ENABLE_TRACKING)

            // Transmit the enable tracking.
            usart_xmit_buffer("ET\r", 3);

            // Set the timer to wait 1 second.
            timer_wait_set(1, 10);

            fsm_checkpoint();

            // Check for ACK.
            fsm_change_state(usart_recv_buffer_has_eol('\r'), ENABLE_TRACKING_ACK);

            // Restart if timer is done.
            fsm_change_state(timer_wait_done(1), PING);

        FSM_STATE_END


        FSM_STATE_BEGIN(ENABLE_TRACKING_ACK)

            // Read the buffer of data.
            usart_recv_buffer(camera_ack, 8, '\r');

            // Start tracking if we got the ack.
            fsm_change_state (!strncmp(camera_ack, "ACK\r", 4), TRACKING);

            // Restart if not ack.
            fsm_change_state(1, PING);

        FSM_STATE_END

        FSM_STATE_BEGIN(TRACKING)

            // Set the timer to wait .2 second.
            timer_wait_set(1, 2);

            fsm_checkpoint();

            // Check for a response packet from the camera.
            fsm_change_state(usart_recv_buffer_has_eol(0xFF), TRACKING_PACKET);

            // Has the timer expired?
            if (timer_wait_done(1))
            {
                // Turn of the tracking LED.
                leds_yellow_off();

                // Reset the blob information.
                blob_size = 0;
                blob_center_x = 88;
                blob_center_y = 72;
            }

        FSM_STATE_END

        FSM_STATE_BEGIN(TRACKING_PACKET)

            // Read the packet of data.
            camera_packet_len = usart_recv_buffer(camera_packet, 64, 0xFF);

            // Process the packet.
            camera_packet_process();

            // Go back to tracking.
            fsm_change_state(1, TRACKING);

        FSM_STATE_END

    FSM_END
}


int main (void)
{
    uint8_t    counter;

    // Initialize the LEDs.
    leds_init();

    // Initialize the timer.
    timer_init();

    // Initialize the motor.
    motors_init();

    // Initialize the sensors.
    sensors_init();

    // Initialize the USART.
    usart_init(BAUD2UBRR_115200);

    // Enable interrupts.
    sei();

    // Set the motors to 50% duty cycle (stopped).
    motors_a_pwm(0);
    motors_b_pwm(0);

    // Reset the counter.
    counter = 0;

    // Loop forever.
    for (;;)
    {
        // Update the sensors.
        sensors_update();

        // Is the timer ready flag set.
        if (timer_is_ready())
        {
            // Increment the counter.
            ++counter;
 
            // Reset the counter at the count of 1 second.
            if (counter == 10) counter = 0;

            // Toggle green LED as needed.
            if (counter == 0) leds_green_on();
            if (counter == 5) leds_green_off();

            // Update the yellow LED to reflect the sensor state.
            // if (sensors_get()) leds_yellow_on(); else leds_yellow_off();

            // Run the finite state machine.
            tablebot_fsm();

            // Clear the timer flag.
            timer_clear_ready();
        }

        // Run the camera finite state machine.
        camera_fsm();
    }

    return 0;
}

