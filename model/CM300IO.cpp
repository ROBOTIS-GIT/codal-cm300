/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.
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
  * Class definition for CM300 IO.
  * Represents a collection of all I/O pins on the device.
  */

#include "CodalConfig.h"
#include "CM300IO.h"
#include "device_pinmap.h"

using namespace codal;

/**
  * Constructor.
  *
  * Create a representation of all given I/O pins on the edge connector
  *
  * Accepts a sequence of unique ID's used to distinguish events raised
  * by NRF52Pin instances on the default EventModel.
  */
CM300IO::CM300IO() :
    //CM300
    led (ID_PIN_P1_04, P1_4, PIN_CAPABILITY_DIGITAL),
    mode_button (ID_PIN_P1_13, P1_13, PIN_CAPABILITY_DIGITAL),
    start_button (ID_PIN_P1_10, P1_10, PIN_CAPABILITY_DIGITAL)
{
}
