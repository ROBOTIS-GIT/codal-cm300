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

/**
  * Class definition for CM300 Serial.
  */

#include "CodalConfig.h"
#include "CM300SAADC.h"

using namespace codal;

/**
 * Constructor.
 *
 * Creates a generic AnalogSensor.
 *
 * @param pin The pin on which to sense
 * @param id The ID of this component e.g. DEVICE_ID_THERMOMETER
 */
CM300SAADC::CM300SAADC(Pin &pin, uint16_t id) : 
    Sensor( id), pin(pin)
{
    updateSample();
}

/**
 * Read the value from pin.
 */
int CM300SAADC::readValue()
{
    return this->pin.getAnalogValue();
}

/**
 * Destructor.
 */
CM300SAADC::~CM300SAADC()
{
}

void CM300SAADC::readDigitalVale(uint8_t data)
{
    this->pin.setDigitalValue(data);
}