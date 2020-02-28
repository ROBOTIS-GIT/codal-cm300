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
  * Class definition for CM300 DYNAMIXEL IO.
  */

#include "CodalConfig.h"
#include "CM300DYNAMIXEL.h"
#include "device_pinmap.h"
#include "Timer.h"

uint32_t dxl_getMillisecond()
{
  return codal::system_timer_current_time();
}

using namespace codal;

/**
  * Constructor.
  */
CM300DYNAMIXEL::CM300DYNAMIXEL()
 :DYNAMIXEL::Master(),
  dxl_tx_(ID_PIN_P0_07, P0_7, PIN_CAPABILITY_DIGITAL), 
  dxl_rx_(ID_PIN_P0_08, P0_8, PIN_CAPABILITY_DIGITAL),
  dxl_dir_(ID_PIN_P1_14, P1_14, PIN_CAPABILITY_DIGITAL),
  dxl_pwr_(ID_PIN_P1_08, P1_8, PIN_CAPABILITY_DIGITAL),
  dxl_serial_(dxl_tx_, dxl_rx_),
  dxl_port_(dxl_serial_, dxl_dir_)
{
  dxl_pwr_.setDigitalValue(1);
  dxl_port_.begin(1000000);
  this->setPort(dxl_port_);
}



