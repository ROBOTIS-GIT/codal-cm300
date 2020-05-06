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

#ifndef CM300_QSPI_FLASH_H
#define CM300_QSPI_FLASH_H

#include "CodalConfig.h"
#include "NRF52Pin.h"
#include "NRF52QSPI.h"
#include "StandardSPIFlash.h"

#define QSPI_FLASH_PAGE_SIZE  256
#define QSPI_FLASH_DENSITY_IN_MBYTES 4
#define QSPI_FLASH_NUM_OF_PAGE  (QSPI_FLASH_DENSITY_IN_MBYTES * 1024 * 1024 / QSPI_FLASH_PAGE_SIZE)

namespace codal
{
    /**
     * Represents a collection of all I/O pins exposed by the device.
     */
    class CM300QSPIFlash
    {
            NRF52Pin sclk_;
            NRF52Pin ss_;
            NRF52Pin io0_;
            NRF52Pin io1_;
            NRF52Pin io2_;
            NRF52Pin io3_;        
            NRF52QSPI qspi_;

            uint32_t number_of_pages_;

        public:
            /**
             * Constructor.
             *
             * Create a representation of all given I/O pins on the edge connector
             *
             * Accepts a sequence of unique ID's used to distinguish events raised
             * by NRF52Pin instances on the default EventModel.
             */
            CM300QSPIFlash();

            virtual int numPages();
            virtual int read(uint32_t addr, uint8_t *rx_buf, uint32_t len);
            
            virtual int write(uint32_t addr, const uint8_t *p_data, uint32_t len);
            virtual int erase(uint32_t addr, uint32_t len);
    };
}

#endif
