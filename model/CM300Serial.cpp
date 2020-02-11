/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

/**
  * Class definition for CM300 IO.
  * Represents a collection of all I/O pins on the device.
  */

#include "CodalConfig.h"
#include "CM300IO.h"
#include "CM300Serial.h"
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
CM300Serial::CM300Serial() :
    //CM300
    tx (ID_PIN_P0_26, P0_26, PIN_CAPABILITY_DIGITAL),
    rx (ID_PIN_P0_27, P0_27, PIN_CAPABILITY_DIGITAL),
    uart (tx, rx, NRF_UARTE0)
{
}
