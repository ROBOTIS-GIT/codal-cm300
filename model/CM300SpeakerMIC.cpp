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
#include "CM300SpeakerMIC.h"
#include "device_pinmap.h"
#include "hal/nrf_gpio.h"
#include "hal/nrf_pwm.h"
#include <stdint.h>

using namespace codal;

/**
  * Constructor.
  */
CM300SpeakerMIC::CM300SpeakerMIC()
  : sck (ID_PIN_P0_13, P0_13, PIN_CAPABILITY_AD),
    ws (ID_PIN_P0_15, P0_15, PIN_CAPABILITY_AD),
    sdout (ID_PIN_P1_00, P1_0, PIN_CAPABILITY_DIGITAL),
    sdin (ID_PIN_P0_24, P0_24, PIN_CAPABILITY_DIGITAL),
    sdmode (ID_PIN_P0_17, P0_17, PIN_CAPABILITY_DIGITAL),
    i2s (sck, ws, nullptr, &sdout, &sdin),
    qspi_flash (),
    mode (CM300_SPEAKER)
{
  memset(buffer[0].mem, 0, MEDIA_BLOCK_SIZE);
  memset(buffer[1].mem, 0, MEDIA_BLOCK_SIZE);

  buffer[0].p_next = &buffer[1];
  buffer[1].p_next = &buffer[0];
  p_buf = &buffer[0];

  nrf_gpio_cfg_output(sdmode.name);
  nrf_gpio_pin_set(sdmode.name);

  i2s.setMode(0); //Master;
  i2s.setSampleRateAndBits(16000);
}


// For record (software I2S)
static uint16_t sck_seq[NRF_PWM_CHANNEL_COUNT] = {0, 0, 0, 0};
static uint16_t ws_seq[NRF_PWM_CHANNEL_COUNT] = {0, 0, 0, 0};
static nrf_pwm_sequence_t sck_pwm = {sck_seq, NRF_PWM_CHANNEL_COUNT, 0, 0};
static nrf_pwm_sequence_t ws_pwm = {ws_seq, NRF_PWM_CHANNEL_COUNT, 0, 0};

void CM300SpeakerMIC::record(uint16_t idx)
{
  if(is_recording)
    return;

  if(mode != CM300_MIC){
    mode = CM300_MIC;

    i2s.setMode(1); //Slave;

    nrf_gpio_pin_clear(sdmode.name);
    nrf_gpio_cfg_output(sck.name);
    nrf_gpio_pin_clear(sck.name);
    nrf_gpio_cfg_output(ws.name);
    nrf_gpio_pin_clear(ws.name);

    nrf_pwm_disable(NRF_PWM1);
    nrf_pwm_disable(NRF_PWM2);

    NRF_PWM1->PSEL.OUT[0] = sck.name;
    NRF_PWM2->PSEL.OUT[0] = ws.name;    

    // For sample rate to 16Khz
    nrf_pwm_configure(NRF_PWM1, NRF_PWM_CLK_16MHz, NRF_PWM_MODE_UP, 15); //SCK
    nrf_pwm_configure(NRF_PWM2, NRF_PWM_CLK_16MHz, NRF_PWM_MODE_UP, 960); //LRCK == Sample Rate
    i2s.setSampleRateAndBits(16000, 24);

    // duty 50%
    sck_seq[0] = 15/2;
    ws_seq[0] = 960/2;

    nrf_pwm_loop_set(NRF_PWM1, 0xFFFF);
    nrf_pwm_loop_set(NRF_PWM2, 0xFFFF);

    nrf_pwm_decoder_set(NRF_PWM1, NRF_PWM_LOAD_INDIVIDUAL, NRF_PWM_STEP_AUTO);
    nrf_pwm_decoder_set(NRF_PWM2, NRF_PWM_LOAD_INDIVIDUAL, NRF_PWM_STEP_AUTO);

    nrf_pwm_sequence_set(NRF_PWM1, 0, &sck_pwm);
    nrf_pwm_sequence_set(NRF_PWM2, 0, &ws_pwm);

    nrf_pwm_int_set(NRF_PWM1, NRF_PWM_INT_LOOPSDONE_MASK);
    nrf_pwm_event_clear(NRF_PWM1, NRF_PWM_EVENT_LOOPSDONE);

    nrf_pwm_int_set(NRF_PWM2, NRF_PWM_INT_LOOPSDONE_MASK);
    nrf_pwm_event_clear(NRF_PWM2, NRF_PWM_EVENT_LOOPSDONE);

    NVIC_SetPriority(PWM1_IRQn, 0);
    NVIC_SetPriority(PWM2_IRQn, 0);
    NVIC_ClearPendingIRQ(PWM1_IRQn);
    NVIC_ClearPendingIRQ(PWM2_IRQn);
    NVIC_EnableIRQ(PWM1_IRQn);
    NVIC_EnableIRQ(PWM2_IRQn);
  }

  nrf_pwm_enable(NRF_PWM1);
  nrf_pwm_enable(NRF_PWM2);  

  repeat_mode = false;

  //For test
  if(idx == 0){
    idx_addr = 0;
    u_buf_size = RECORD_SIZE;
  }else{
    return;
  }

  u_buf_remain = u_buf_size;
  qspi_flash.erase(idx_addr, u_buf_size);
   
  is_recording = true;
  i2s.start(nullptr, (uint32_t*)p_buf->mem,
    MEDIA_BLOCK_SIZE/4, (DoublePVoidCallback)&_doneHandler, this);

  nrf_pwm_task_trigger(NRF_PWM1, NRF_PWM_TASK_SEQSTART0);
  nrf_pwm_task_trigger(NRF_PWM2, NRF_PWM_TASK_SEQSTART0);

  while(is_recording){
    if(is_buf_updated){
      pullBuffer();
      is_buf_updated = false;
    }
  }
}

void CM300SpeakerMIC::play(uint16_t idx, bool repeat)
{
  if(is_recording)
    return;

  repeat_mode = repeat;

  if(mode != CM300_SPEAKER){
    mode = CM300_SPEAKER;

    nrf_pwm_disable(NRF_PWM1);
    nrf_pwm_disable(NRF_PWM2);
    nrf_gpio_pin_set(sdmode.name);
    is_recording = false;

    i2s.setMode(0); //Master;
  }
 
  // TODO: Copy the wav information at a specific address(separated by index)
  // of SPI Flash to the internal buffer.
  uint32_t sample_rate;
  uint8_t bits;

  // For test
  if(idx == 0){
    idx_addr = 0;
    u_buf_size = RECORD_SIZE;
    sample_rate = 16000;
    bits = 16;
  }else{
    return;
  }

  i2s.setSampleRateAndBits(sample_rate, bits);
  u_buf_remain = u_buf_size;

  pushBuffer();
  i2s.start((const uint32_t*)p_buf->mem, nullptr,
    MEDIA_BLOCK_SIZE/4, (DoublePVoidCallback)&_doneHandler, this);
}

void CM300SpeakerMIC::_doneHandler(void* fixed_arg, void* _self)
{
  static NRF52I2S* i2s = (NRF52I2S*)fixed_arg;
  static CM300SpeakerMIC* self = (CM300SpeakerMIC*)_self;
  
  if(!self->repeat_mode
    && !self->u_buf_remain
    && !i2s->isFirstBufferFromStart())
  {
    i2s->stop();
    if(self->mode == CM300_MIC){
      nrf_pwm_disable(NRF_PWM1);
      nrf_pwm_disable(NRF_PWM2);
      self->is_recording = false;
    }
  }else if(self->u_buf_remain > 0){
    //Swap internal double buffer for next buffer.
    self->p_buf = self->p_buf->p_next;

    if(self->mode == CM300_SPEAKER){
      i2s->setNextBuffers((const uint32_t*)self->p_buf->mem, nullptr);
      // TODO: Copy the wav information at a specific address(separated by index)
      // of SPI Flash to the internal buffer.

      // For test
      self->pushBuffer();
    }else if(self->mode == CM300_MIC){
      i2s->setNextBuffers(nullptr, (uint32_t*)self->p_buf->mem);
      // TODO: Copy the wav information at the internal buffer 
      // to a specific address(separated by index) of SPI Flash

      // For test
      if(!i2s->isFirstBufferFromStart()){
        self->is_buf_updated = true;
      }
    }
  }
}

void CM300SpeakerMIC::pushBuffer()
{
  // Push data to next buffer
  if(u_buf_remain >= MEDIA_BLOCK_SIZE){
    qspi_flash.read(idx_addr, p_buf->mem, MEDIA_BLOCK_SIZE);      
    idx_addr += MEDIA_BLOCK_SIZE;
    u_buf_remain -= MEDIA_BLOCK_SIZE;
  }else{
    qspi_flash.read(idx_addr, p_buf->mem, u_buf_remain);
    memset(p_buf->mem+u_buf_remain, 0, (MEDIA_BLOCK_SIZE-u_buf_remain));
    idx_addr += u_buf_remain;
    if(repeat_mode){
      // Restore to original address
      idx_addr -= u_buf_size;
      u_buf_remain = u_buf_size;
    }else{
      u_buf_remain = 0;
    }
  }
}

void CM300SpeakerMIC::pullBuffer()
{
  static uint32_t block_words = MEDIA_BLOCK_SIZE/4;
  static uint32_t block_half_words = MEDIA_BLOCK_SIZE/2;

  // Pull data to next buffer
  int32_t* p_s32 = (int32_t*)p_buf->mem;
  int16_t* p_s16 = (int16_t*)p_s32;
  for(uint32_t i = 0; i < block_words; i++)
  {
    p_s16[i] = ((int16_t)(p_s32[i] >> 9))<<3; // 3 to make the recording sound louder.
  }
  qspi_flash.write(idx_addr, (const uint8_t*)p_s16, block_half_words);
  
  if(u_buf_remain >= block_half_words){
    idx_addr += block_half_words;
    u_buf_remain -= block_half_words;
  }else{
    u_buf_remain = 0;
  }
}



extern "C" void PWM1_IRQHandler()
{
    // The user handler is called for SEQEND0 and SEQEND1 events only when the
    // user asks for it (by setting proper flags when starting the playback).
    if (nrf_pwm_event_check(NRF_PWM1, NRF_PWM_EVENT_LOOPSDONE)){
        nrf_pwm_seq_ptr_set(NRF_PWM1, 0, sck_seq);
        nrf_pwm_seq_cnt_set(NRF_PWM1, 0, NRF_PWM_CHANNEL_COUNT);
        nrf_pwm_task_trigger(NRF_PWM1, NRF_PWM_TASK_SEQSTART0);
        nrf_pwm_event_clear(NRF_PWM1, NRF_PWM_EVENT_LOOPSDONE);
        // Spurious read to cover the "events don't clear immediately" erratum
        (void)nrf_pwm_event_check(NRF_PWM1, NRF_PWM_EVENT_LOOPSDONE);            
    }
}

extern "C" void PWM2_IRQHandler()
{
    // The user handler is called for SEQEND0 and SEQEND1 events only when the
    // user asks for it (by setting proper flags when starting the playback).
    if (nrf_pwm_event_check(NRF_PWM2, NRF_PWM_EVENT_LOOPSDONE)){
        nrf_pwm_seq_ptr_set(NRF_PWM2, 0, ws_seq);
        nrf_pwm_seq_cnt_set(NRF_PWM2, 0, NRF_PWM_CHANNEL_COUNT);
        nrf_pwm_task_trigger(NRF_PWM2, NRF_PWM_TASK_SEQSTART0);
        nrf_pwm_event_clear(NRF_PWM2, NRF_PWM_EVENT_LOOPSDONE);
        // Spurious read to cover the "events don't clear immediately" erratum
        (void)nrf_pwm_event_check(NRF_PWM2, NRF_PWM_EVENT_LOOPSDONE);
    }
}
