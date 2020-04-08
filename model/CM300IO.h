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

#ifndef CM300_IO_H
#define CM300_IO_H

#include "CodalConfig.h"
#include "NRF52Pin.h"

//
// Component IDs for each pin.
// The can be user defined, but uniquely identify a pin when using the eventing APIs/
//
#define ID_PIN_P0_00        (DEVICE_ID_IO_P0 + 0)
#define ID_PIN_P0_01        (DEVICE_ID_IO_P0 + 1)
#define ID_PIN_P0_02        (DEVICE_ID_IO_P0 + 2)
#define ID_PIN_P0_03        (DEVICE_ID_IO_P0 + 3)
#define ID_PIN_P0_04        (DEVICE_ID_IO_P0 + 4)
#define ID_PIN_P0_05        (DEVICE_ID_IO_P0 + 5)
#define ID_PIN_P0_06        (DEVICE_ID_IO_P0 + 6)
#define ID_PIN_P0_07        (DEVICE_ID_IO_P0 + 7)
#define ID_PIN_P0_08        (DEVICE_ID_IO_P0 + 8)
#define ID_PIN_P0_09        (DEVICE_ID_IO_P0 + 9)
#define ID_PIN_P0_10        (DEVICE_ID_IO_P0 + 10)
#define ID_PIN_P0_11        (DEVICE_ID_IO_P0 + 11)
#define ID_PIN_P0_12        (DEVICE_ID_IO_P0 + 12)
#define ID_PIN_P0_13        (DEVICE_ID_IO_P0 + 13)
#define ID_PIN_P0_14        (DEVICE_ID_IO_P0 + 14)
#define ID_PIN_P0_15        (DEVICE_ID_IO_P0 + 15)
#define ID_PIN_P0_16        (DEVICE_ID_IO_P0 + 16)
#define ID_PIN_P0_17        (DEVICE_ID_IO_P0 + 17)
#define ID_PIN_P0_18        (DEVICE_ID_IO_P0 + 18)
#define ID_PIN_P0_19        (DEVICE_ID_IO_P0 + 19)
#define ID_PIN_P0_20        (DEVICE_ID_IO_P0 + 20)
#define ID_PIN_P0_21        (DEVICE_ID_IO_P0 + 21)
#define ID_PIN_P0_22        (DEVICE_ID_IO_P0 + 22)
#define ID_PIN_P0_23        (DEVICE_ID_IO_P0 + 23)
#define ID_PIN_P0_24        (DEVICE_ID_IO_P0 + 24)
#define ID_PIN_P0_25        (DEVICE_ID_IO_P0 + 25)
#define ID_PIN_P0_26        (DEVICE_ID_IO_P0 + 26)
#define ID_PIN_P0_27        (DEVICE_ID_IO_P0 + 27)
#define ID_PIN_P0_28        (DEVICE_ID_IO_P0 + 28)
#define ID_PIN_P0_29        (DEVICE_ID_IO_P0 + 29)
#define ID_PIN_P0_30        (DEVICE_ID_IO_P0 + 30)
#define ID_PIN_P0_31        (DEVICE_ID_IO_P0 + 31)

#define DEVICE_ID_IO_P1     (ID_PIN_P0_31 + 1)
#define ID_PIN_P1_00        (DEVICE_ID_IO_P1 + 0)
#define ID_PIN_P1_01        (DEVICE_ID_IO_P1 + 1)
#define ID_PIN_P1_02        (DEVICE_ID_IO_P1 + 2)
#define ID_PIN_P1_03        (DEVICE_ID_IO_P1 + 3)
#define ID_PIN_P1_04        (DEVICE_ID_IO_P1 + 4)
#define ID_PIN_P1_05        (DEVICE_ID_IO_P1 + 5)
#define ID_PIN_P1_06        (DEVICE_ID_IO_P1 + 6)
#define ID_PIN_P1_07        (DEVICE_ID_IO_P1 + 7)
#define ID_PIN_P1_08        (DEVICE_ID_IO_P1 + 8)
#define ID_PIN_P1_09        (DEVICE_ID_IO_P1 + 9)
#define ID_PIN_P1_10        (DEVICE_ID_IO_P1 + 10)
#define ID_PIN_P1_11        (DEVICE_ID_IO_P1 + 11)
#define ID_PIN_P1_12        (DEVICE_ID_IO_P1 + 12)
#define ID_PIN_P1_13        (DEVICE_ID_IO_P1 + 13)
#define ID_PIN_P1_14        (DEVICE_ID_IO_P1 + 14)
#define ID_PIN_P1_15        (DEVICE_ID_IO_P1 + 15)


#define DK_BUILT_LED_CNT     4
#define DK_BUILT_BUTTON_CNT  4

namespace codal
{
    /**
     * Represents a collection of all I/O pins exposed by the device.
     */
    class CM300IO
    {
        public:
            NRF52Pin          led;
            NRF52Pin          mode_button;
            NRF52Pin          start_button;   

            /**
             * Constructor.
             *
             * Create a representation of all given I/O pins on the edge connector
             *
             * Accepts a sequence of unique ID's used to distinguish events raised
             * by NRF52Pin instances on the default EventModel.
             */
            CM300IO();
    };
}

#endif
