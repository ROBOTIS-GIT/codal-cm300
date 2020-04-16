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

#ifndef CM300_SPKEAR_MIC_H
#define CM300_SPKEAR_MIC_H

#include "CodalConfig.h"
#include "NRF52Pin.h"
#include "NRF52I2S.h"
#include "CM300QSPIFlash.h"

// It must be 4 bytes unconditionally (alignment is also important).
#define MEDIA_BLOCK_SIZE    (4*1024)
#define RECORD_SIZE         (4*1024*64)

namespace codal
{
    typedef enum {
        CM300_SPEAKER = 0,
        CM300_MIC
    } SpeakerMicMode_t;

    typedef struct InternalBuffer {
        struct InternalBuffer *p_next;
        uint8_t mem[MEDIA_BLOCK_SIZE];
    } InternalBuffer_t;

    class CM300SpeakerMIC
    {
        NRF52Pin sck;
        NRF52Pin ws;
        NRF52Pin sdout;
        NRF52Pin sdin;
        NRF52Pin sdmode;
        NRF52I2S i2s;
        CM300QSPIFlash qspi_flash;

        SpeakerMicMode_t mode;
        bool repeat_mode; //Only Speaker
        volatile bool is_recording;
        volatile bool is_buf_updated;

        InternalBuffer_t *p_buf;
        InternalBuffer_t buffer[2];

        uint32_t u_buf_size;
        volatile uint32_t u_buf_remain;
        volatile uint32_t idx_addr;

        static void _doneHandler(void* fixed_arg, void* _self);
        void pushBuffer();
        void pullBuffer();

        public:
            /**
             * Constructor.
             */
            CM300SpeakerMIC();

            void record(uint16_t idx);
            void play(uint16_t idx, bool repeat = true);
    };
}

#endif
