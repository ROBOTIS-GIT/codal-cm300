/*******************************************************************************
* Copyright 2016 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef CM300_JOY_STICK_H
#define CM300_JOY_STICK_H

#include "CodalConfig.h"
#include "CM300IO.h"
#include "Event.h"
#include "Sensor.h"


namespace codal
{
    class CM300Joystick : public Sensor
    {   
        Pin&      pinX;              // Pin where the sensor is connected.
        Pin&      pinY;              // Pin where the sensor is connected.
        uint32_t   temp;

        public:

        /**
          * Constructor.
          *
          * Creates a generic AnalogSensor.
          *
          * @param pin The pin on which to sense
          * @param id The ID of this compoenent e.g.
         */
        CM300Joystick(Pin &pinX, uint16_t id1, Pin &pinY, uint16_t id2);

        /**
         * Read the value from pin.
         */
        // virtual int readValue(uint8_t Num);
        virtual int readValue();

        /**
          * Destructor.
          */
        ~CM300Joystick();
    };
}


#endif