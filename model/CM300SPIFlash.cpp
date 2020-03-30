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
  * Class definition for CM300 SPI Flash.
  * Represents the spi flash on the device.
  */

#include "CodalConfig.h"
#include "CM300IO.h"
#include "CM300SPIFlash.h"
#include "device_pinmap.h"

#define check(cond)                                                    \
    if (!(cond))                                                       \
    target_panic(909)

using namespace codal;

/**
  * Constructor.
  *
  * Create a representation of the spi flash on the device
  *
  * Since there is only one SPI Flash on the device,
  * fix the SPI peripheral as SPIM0 and baudrate as 8Mbps.
  */
CM300SPIFlash::CM300SPIFlash()
 :SPIFlash(),
  mosi_ (ID_PIN_P0_21, P0_21, PIN_CAPABILITY_DIGITAL),
  miso_ (ID_PIN_P0_23, P0_23, PIN_CAPABILITY_DIGITAL),
  sclk_ (ID_PIN_P0_19, P0_19, PIN_CAPABILITY_DIGITAL),
  cs_ (ID_PIN_P1_15, P1_15, PIN_CAPABILITY_DIGITAL),
  flash_spi_ (mosi_, miso_, sclk_, NRF_SPIM0),
  number_of_pages_ (SPI_FLASH_NUM_OF_PAGE),
  status_ (DEVICE_OK),
  transfer_done_ (false)
{
  cs_.setDigitalValue(1);
  flash_spi_.setFrequency(8000000);
}

//Static
void CM300SPIFlash::irqDoneHandler_(void *self_)
{
  CM300SPIFlash *self = (CM300SPIFlash *)self_;

  self->setTransferDoneFlag(true);
}

int CM300SPIFlash::getStatusRegister1(uint8_t *status)
{
  uint8_t ret;

  setCommand(0x05, -1);

  ret = transfer(command_buffer_, 1, read_buffer_, 2,
                                 irqDoneHandler_, this);

  if (ret == DEVICE_OK)
  {
    *status = read_buffer_[1];
  }

  return ret;
}

void CM300SPIFlash::setCommand(uint8_t command, int addr)
{
    command_buffer_[0] = command;
    command_buffer_[1] = addr >> 16;
    command_buffer_[2] = addr >> 8;
    command_buffer_[3] = addr >> 0;
}

void CM300SPIFlash::setTxBuffer(uint8_t *tx_buffer, uint8_t command, int addr,
                                 uint8_t *data_buffer, uint16_t data_length)
{
  uint16_t index = 0;

  tx_buffer[index++] = command;

  if (addr != -1)
  {
    tx_buffer[index++] = addr >> 16;
    tx_buffer[index++] = addr >> 8;
    tx_buffer[index++] = addr >> 0;
  }

  if (data_length)
  {

    for (uint16_t i = 0; i < data_length; i++)
    {
      tx_buffer[index++] = data_buffer[i];
    }
  }
}

int CM300SPIFlash::transfer(uint8_t const *p_tx_buffer, uint32_t tx_length, uint8_t *p_rx_buffer,
                         uint32_t rx_length, PVoidCallback doneHandler, void *arg)
{
  int ret = DEVICE_OK;

  cs_.setDigitalValue(0);
  setTransferDoneFlag(false);
  ret = flash_spi_.startTransfer(p_tx_buffer, tx_length, p_rx_buffer, rx_length, doneHandler, arg);
  while(getTransferDoneFlag() == false);
  cs_.setDigitalValue(1);

  return ret;
}

void CM300SPIFlash::writeDisable()
{
  getStatusRegister1(&status_);

  if ((status_ & 0x02) == 0x02)
  {
    command_buffer_[0] = 0x04;

    transfer(command_buffer_, 1, NULL, 0, irqDoneHandler_, this);
  }
}

void CM300SPIFlash::writeEnable()
{
  getStatusRegister1(&status_);

  if ((status_ & 0x02) == 0)
  {
    command_buffer_[0] = 0x06;

    transfer(command_buffer_, 1, NULL, 0, irqDoneHandler_, this);
  }
}

int CM300SPIFlash::waitBusy(int waitMS)
{
  do
  {
    int r = getStatusRegister1(&status_);
    if (r < 0)
      return r;
//    if (waitMS)
//      fiber_sleep(waitMS);
  } while (status_ & 0x02);

  return DEVICE_OK;
}

int CM300SPIFlash::numPages()
{
  return number_of_pages_;
}

int CM300SPIFlash::readBytes(uint32_t addr, void *buffer, uint32_t len)
{
  check(addr + len <= number_of_pages_ * SPIFLASH_PAGE_SIZE);
  check(addr <= number_of_pages_ * SPIFLASH_PAGE_SIZE);

  int ret = DEVICE_OK;

  setTxBuffer(write_buffer_, 0x03, addr, NULL, 0);

  waitBusy(1);

  ret = transfer(write_buffer_, 4, (uint8_t *)buffer, len, irqDoneHandler_, this);

  return ret;
}

int CM300SPIFlash::writePage(uint32_t addr, uint8_t *buffer, uint32_t len)
{
  check(len <= SPIFLASH_PAGE_SIZE);
  check(addr / SPIFLASH_PAGE_SIZE == (addr + len - 1) / SPIFLASH_PAGE_SIZE);
  check(addr + len <= number_of_pages_ * SPIFLASH_PAGE_SIZE);

  int ret = DEVICE_OK;

  setTxBuffer(write_buffer_, 0x02, addr, buffer, len);

  waitBusy(1);

  writeEnable();

  ret = transfer(write_buffer_, len + 4, NULL, 0, irqDoneHandler_, this);

  return ret;
}

int CM300SPIFlash::writeBytes(uint32_t addr, const void *buffer, uint32_t len)
{
  check(addr + len <= number_of_pages_ * SPIFLASH_PAGE_SIZE);

  uint8_t *p_buffer = (uint8_t *)buffer;

  while (len > 256)
  {
    writePage(addr, p_buffer, 256);
    addr += 256;
    p_buffer += 256;
    len -= 256;
  }

  writePage(addr, p_buffer, len);

  return DEVICE_OK;
}

int CM300SPIFlash::eraseSmallRow(uint32_t addr)
{
  int ret = DEVICE_OK;

  check(addr <= number_of_pages_ * SPIFLASH_PAGE_SIZE);
  check((addr & (SPIFLASH_SMALL_ROW_SIZE - 1)) == 0);

  waitBusy(1);

  writeEnable();

  setCommand(0x20, addr);

  ret = transfer(command_buffer_, 4, NULL, 0, irqDoneHandler_, this);

  return ret;
}

int CM300SPIFlash::eraseBigRow(uint32_t addr)
{
  check(addr <= number_of_pages_ * SPIFLASH_PAGE_SIZE);
  check((addr & (SPIFLASH_SMALL_ROW_SIZE - 1)) == 0);

  int ret = DEVICE_OK;

  waitBusy(1);

  writeEnable();

  setCommand(0xD8, addr);

  ret = transfer(command_buffer_, 4, NULL, 0, irqDoneHandler_, this);

  return ret;
}

int CM300SPIFlash::eraseChip()
{
  int ret = DEVICE_OK;

  waitBusy(1);

  writeEnable();

  command_buffer_[0] = 0xC7;

  ret = transfer(command_buffer_, 1, NULL, 0, irqDoneHandler_, this);

  return ret;
}

void CM300SPIFlash::setTransferDoneFlag(bool flag)
{
  transfer_done_ = flag;
}

bool CM300SPIFlash::getTransferDoneFlag(void)
{
  return transfer_done_;
}
