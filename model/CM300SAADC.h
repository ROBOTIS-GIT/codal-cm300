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

#ifndef CM300_SAADC_H
#define CM300_SAADC_H

#include "CodalConfig.h"
#include "CM300IO.h"
#include "Event.h"
#include "Sensor.h"


namespace codal
{
    class CM300SAADC : public Sensor
    {   

        Pin&      pin;              // Pin where the sensor is connected.
        public:

        /**
          * Constructor.
          *
          * Creates a generic AnalogSensor.
          *
          * @param pin The pin on which to sense
          * @param id The ID of this compoenent e.g.
         */
        CM300SAADC(Pin &pin, uint16_t id);

        /**
         * Read the value from pin.
         */
        virtual int readValue();

        /**
          * Destructor.
          */
        ~CM300SAADC();

        void readDigitalVale(uint8_t data);
    };
}


#endif