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

#ifndef CM300_SPI_FLASH_H
#define CM300_SPI_FLASH_H

#include "CodalConfig.h"
#include "NRF52Pin.h"
#include "NRF52SPI.h"
#include "StandardSPIFlash.h"

#define SPI_FLASH_DENSITY_IN_MBYTES 8
#define SPI_FLASH_NUM_OF_PAGE       (SPI_FLASH_DENSITY_IN_MBYTES * 1024 * 1024 / SPIFLASH_PAGE_SIZE)

#define SPI_FLASH_BUFFER_MAX_SIZE   (4 + 2048)

namespace codal
{
    /**
     * Represents a collection of all I/O pins exposed by the device.
     */
    class CM300SPI_Flash : public SPIFlash
    {
            NRF52Pin          mosi_;
            NRF52Pin          miso_;
            NRF52Pin          sclk_;
            NRF52Pin          cs_;
            NRF52SPI          flash_spi_;
            uint32_t          number_of_pages_;
            uint8_t           command_buffer_[4];
            uint8_t           write_buffer_[260];
            uint8_t           read_buffer_[SPI_FLASH_BUFFER_MAX_SIZE];
            uint8_t           status_;
            volatile bool     transfer_done_;

            void setCommand(uint8_t command, int addr);
            void setTxBuffer(uint8_t *tx_buffer, uint8_t command, int addr,
                             uint8_t *data_buffer, uint16_t data_length);
            int transfer(uint8_t const *p_tx_buffer, uint32_t tx_length, uint8_t *p_rx_buffer,
                         uint32_t rx_length, PVoidCallback doneHandler, void *arg);
            int getStatusRegister1(uint8_t *status);
            int waitBusy(int waitMS);
            void writeEnable();
            int writePage(uint32_t addr, uint8_t *buffer, uint32_t len);
            static void irqDoneHandler_(void *self_);
            void setTransferDoneFlag(bool flag);
            bool getTransferDoneFlag(void);

        public:


            /**
             * Constructor.
             *
             * Create a representation of all given I/O pins on the edge connector
             *
             * Accepts a sequence of unique ID's used to distinguish events raised
             * by NRF52Pin instances on the default EventModel.
             */
            CM300SPI_Flash();

            virtual int numPages();
            virtual int readBytes(uint32_t addr, void *buffer, uint32_t len);
            virtual int writeBytes(uint32_t addr, const void *buffer, uint32_t len);
            virtual int eraseSmallRow(uint32_t addr);
            virtual int eraseBigRow(uint32_t addr);
            virtual int eraseChip();
            void writeDisable();
    };
}

#endif
