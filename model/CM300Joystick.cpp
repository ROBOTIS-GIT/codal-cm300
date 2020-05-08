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
#include "CM300Joystick.h"
#include "device_pinmap.h"

using namespace codal;

/**
 * Constructor.
 *
 * Creates a generic AnalogSensor.
 *
 * @param pin1  The pin on which to sense
 * @param id2   The ID of this component e.g. DEVICE_ID_IO_P0
 * @param pin2  The pin on which to enable
 * @param id2   The ID of this component e.g. DEVICE_ID_IO_P0
 */
//Pin(int id, PinNumber name, PinCapability capability)
CM300Joystick::CM300Joystick(Pin &pinX, uint16_t id1, Pin &pinY, uint16_t id2) :
    Sensor( id), pinX(pinX), pinY(pinY)
{
    updateSample();
}

/**
 * Pure virtual function.
 *
 * Read analog Velue from sensor
 * 
 */
// int CM300SAADC::readValue(uint8_t Num) 
int CM300Joystick::readValue()
{
    temp = this->pinX.getAnalogValue();
    temp = temp << 16; //32Bit upper16Bit for X, Lower 16Bit for Y
    temp = temp|this->pinY.getAnalogValue();

    return temp;
}

/**
 * Destructor.
 */
CM300Joystick::~CM300Joystick()
{
}