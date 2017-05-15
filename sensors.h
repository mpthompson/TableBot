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

#ifndef _TB_SENSORS_H_
#define _TB_SENSORS_ 1

#define SENSOR_LEFT_FRONT           0
#define SENSOR_RIGHT_FRONT          1
#define SENSOR_GROUND_FRONT         2
#define SENSOR_GROUND_LEFT_FRONT    3
#define SENSOR_GROUND_RIGHT_FRONT   4
#define SENSOR_GROUND_LEFT_REAR     5
#define SENSOR_GROUND_RIGHT_REAR    6

#define SENSORS_FORWARD ((1<<SENSOR_LEFT_FRONT) | (1<<SENSOR_RIGHT_FRONT) | (1<<SENSOR_GROUND_FRONT) | (1<<SENSOR_GROUND_LEFT_FRONT) | (1<<SENSOR_GROUND_RIGHT_FRONT))
#define SENSORS_REARWARD ((1<<SENSOR_GROUND_LEFT_REAR) | (1<<SENSOR_GROUND_RIGHT_REAR))

#define SENSORS_FORWARD_GROUND(sensors) ((sensors & ((1<<SENSOR_LEFT_FRONT) | (1<<SENSOR_RIGHT_FRONT) | (1<<SENSOR_GROUND_FRONT) | (1<<SENSOR_GROUND_LEFT_FRONT) | (1<<SENSOR_GROUND_RIGHT_FRONT))) == 0)
#define SENSORS_REARWARD_GROUND(sensors) ((sensors & ((1<<SENSOR_GROUND_LEFT_REAR) | (1<<SENSOR_GROUND_RIGHT_REAR))) == 0)
#define SENSORS_LEFTWARD_GROUND(sensors) ((sensors & ((1<<SENSOR_GROUND_LEFT_FRONT) | (1<<SENSOR_GROUND_RIGHT_REAR))) == 0)
#define SENSORS_RIGHTWARD_GROUND(sensors) ((sensors & ((1<<SENSOR_GROUND_RIGHT_FRONT) | (1<<SENSOR_GROUND_LEFT_REAR))) == 0)

void sensors_init(void);
void sensors_update(void);
uint8_t sensors_get(void);
uint8_t sensors_triggered(uint8_t sensors_mask);

#endif // _TB_SENSORS_H_
