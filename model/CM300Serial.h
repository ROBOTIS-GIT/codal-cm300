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

#ifndef CM300_SERIAL_H
#define CM300_SERIAL_H

#include "CodalConfig.h"
#include "NRF52Pin.h"
#include "NRF52Serial.h"

namespace codal
{
    /**
     * Represents a collection of all I/O pins exposed by the device.
     */
    class CM300Serial
    {
        public:
            NRF52Pin          tx;
            NRF52Pin          rx;
            NRF52Serial       uart;

            /**
             * Constructor.
             *
             * Create a representation of all given I/O pins on the edge connector
             *
             * Accepts a sequence of unique ID's used to distinguish events raised
             * by NRF52Pin instances on the default EventModel.
             */
            CM300Serial();
    };
}

#endif
