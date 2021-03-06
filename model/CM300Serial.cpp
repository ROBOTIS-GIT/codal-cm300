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
#include "CM300IO.h"
#include "CM300Serial.h"
#include "device_pinmap.h"

using namespace codal;

/**
  * Constructor.
  */
CM300Serial::CM300Serial() :
    //CM300
    NRF52Serial(tx, rx),
    tx (ID_PIN_P0_26, P0_26, PIN_CAPABILITY_DIGITAL),
    rx (ID_PIN_P0_27, P0_27, PIN_CAPABILITY_DIGITAL)
{
    configurePins(tx, rx);
    setBaudrate(1000000);
}
