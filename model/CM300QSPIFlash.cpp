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
#include "CM300QSPIFlash.h"
#include "device_pinmap.h"

using namespace codal;

/**
  * Constructor.
  *
  * Create a representation of the spi flash on the device
  */
CM300QSPIFlash::CM300QSPIFlash()
 :sclk_ (ID_PIN_P0_19, P0_19, PIN_CAPABILITY_DIGITAL),
  ss_ (ID_PIN_P1_15, P1_15, PIN_CAPABILITY_DIGITAL),
  io0_ (ID_PIN_P0_21, P0_21, PIN_CAPABILITY_DIGITAL),
  io1_ (ID_PIN_P0_23, P0_23, PIN_CAPABILITY_DIGITAL),
  io2_ (ID_PIN_P0_18, P0_18, PIN_CAPABILITY_DIGITAL), //Dummy
  io3_ (ID_PIN_P0_18, P0_18, PIN_CAPABILITY_DIGITAL), //Dummy 
  qspi_ (sclk_, ss_, io0_, io1_, io2_, io3_),
  number_of_pages_ (QSPI_FLASH_NUM_OF_PAGE)
{
  qspi_.setFrequency(32000000);
  qspi_.configureFormat(QSPI_CFG_BUS_SINGLE, QSPI_CFG_BUS_SINGLE, 
        QSPI_CFG_ADDR_BITS_24, QSPI_CFG_BUS_DUAL, 
        8, QSPI_CFG_BUS_DUAL, 0);

  // For AT25SF321
  qspi_.configureReadFormat(QSPI_CFG_BUS_DUAL, QSPI_CFG_BUS_DUAL);
  qspi_.configureWriteFormat(QSPI_CFG_BUS_SINGLE, QSPI_CFG_BUS_SINGLE);
}


int CM300QSPIFlash::numPages()
{
  return number_of_pages_;
}


int CM300QSPIFlash::read(uint32_t addr, uint8_t *rx_buf, uint32_t len)
{
  return qspi_.read(addr, rx_buf, len);
}


int CM300QSPIFlash::write(uint32_t addr, const uint8_t *p_data, uint32_t len)
{
  int ret;
  uint32_t index;
  uint32_t write_length;
  uint32_t write_addr;
  uint8_t t_mem[QSPI_FLASH_PAGE_SIZE]; // For page-by-page processing. 
  uint8_t* p_buf;
  uint32_t offset;

  index = 0;
  offset = addr % QSPI_FLASH_PAGE_SIZE;

  if (offset > 0 || len < QSPI_FLASH_PAGE_SIZE)
  {
    p_buf = t_mem;
    write_addr = addr - offset;
    write_length = QSPI_FLASH_PAGE_SIZE-offset;
    if(write_length > len){
      write_length = len;
    }
    qspi_.read(write_addr, p_buf, QSPI_FLASH_PAGE_SIZE);
    memcpy(p_buf+offset, p_data, write_length);

    ret = qspi_.write(write_addr, p_buf, QSPI_FLASH_PAGE_SIZE);
    if (ret != DEVICE_OK){
      return ret;
    }

    if (len < QSPI_FLASH_PAGE_SIZE){
      index += len;
    }else{
      index += (QSPI_FLASH_PAGE_SIZE - offset);
    }
  }

  while(index < len)
  {
    write_length = len - index;
    if(write_length > QSPI_FLASH_PAGE_SIZE){
      write_length = QSPI_FLASH_PAGE_SIZE;
    }

    write_addr = addr + index;
    p_buf = (uint8_t*)&p_data[index];
    ret = qspi_.write(write_addr, p_buf, QSPI_FLASH_PAGE_SIZE);
    if (ret != DEVICE_OK){
      break;
    }    
    index += write_length;

    p_buf = t_mem;
    if ((len - index) > 0 && (len - index) < QSPI_FLASH_PAGE_SIZE){
      offset = len - index;
      write_addr = addr + index;
      qspi_.read(write_addr, p_buf, QSPI_FLASH_PAGE_SIZE);
      memcpy(p_buf, &p_data[index], offset);

      ret = qspi_.write(write_addr, p_buf, QSPI_FLASH_PAGE_SIZE);
      break;
    }
  }

  return ret;
}


int CM300QSPIFlash::erase(uint32_t addr, uint32_t len)
{
  int ret = DEVICE_OK;
  uint32_t erase_addr;
  uint32_t offset = addr % 4096;
  uint32_t remain_len = len;
  uint16_t erase_cnt = 0;

  if((addr+len) > (QSPI_FLASH_PAGE_SIZE * QSPI_FLASH_NUM_OF_PAGE)){
    return DEVICE_INVALID_PARAMETER;
  }

  erase_addr = addr - offset;
  remain_len += offset;

  while(remain_len){
    if(erase_addr % sizeof(uint16_t) && remain_len >= sizeof(uint16_t)){
      erase_cnt = remain_len/sizeof(uint16_t);
      ret = qspi_.erase64K(erase_addr, erase_cnt);
      if(ret != DEVICE_OK){
        break;
      }
      remain_len -= erase_cnt*sizeof(uint16_t);
      erase_addr += erase_cnt*sizeof(uint16_t);
    }else if(remain_len >= 4096){
      erase_cnt = remain_len/4096;
      ret = qspi_.erase4K(erase_addr, erase_cnt);
      if(ret != DEVICE_OK){
        break;
      }      
      remain_len -= erase_cnt*4096;
      erase_addr += erase_cnt*4096;
    }else{
      ret = qspi_.erase4K(erase_addr, 1);
      if(ret != DEVICE_OK){
        break;
      }      
      remain_len = 0;
    }
  }

  return ret;
}
