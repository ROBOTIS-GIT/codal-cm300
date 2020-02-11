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


#include "CM300.h"
#include "Timer.h"
#include "nrf.h"

using namespace codal;

CM300 *cm300_device_instance = NULL;

/**
  * Constructor.
  *
  * Create a representation of a CM300 device, which includes member variables
  * that represent various device drivers used to control aspects of the CM300.
  */
CM300::CM300() :
    io(),
    timer1(NRF_TIMER1, TIMER1_IRQn),
    timer(timer1),
    messageBus()
{
    // Clear our status
    status = 0;
    cm300_device_instance = this;

    // Ensure NFC pins are configured as GPIO. If not, update the non-volatile UICR.
    if (NRF_UICR->NFCPINS)
    {
        DMESG("RESET UICR\n");
        // Enable Flash Writes
        NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

        // Configure PINS for GPIO use.
        NRF_UICR->NFCPINS = 0;

        // Disable Flash Writes
        NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

        // Reset, so the changes can take effect.
        NVIC_SystemReset();
    }

    // Configure serial port for debugging
    //serial.set_flow_control(mbed::Serial::Disabled);
    // serial.baud(115200);
}

/**
  * Post constructor initialisation method.
  *
  * This call will initialised the scheduler, memory allocator and Bluetooth stack.
  *
  * This is required as the Bluetooth stack can't be brought up in a
  * static context i.e. in a constructor.
  *
  * @code
  * CM300 cm;
  * cm.init();
  * @endcode
  *
  * @note This method must be called before user code utilises any functionality
  *       contained within the GenuinoZero class.
  */
int CM300::init()
{
    if (status & DEVICE_INITIALIZED)
        return DEVICE_NOT_SUPPORTED;

    status |= DEVICE_INITIALIZED;

    // Bring up fiber scheduler.
    scheduler_init(messageBus);

    for(int i = 0; i < DEVICE_COMPONENT_COUNT; i++)
    {
        if(CodalComponent::components[i])
            CodalComponent::components[i]->init();
    }

    // Seed our random number generator
    //seedRandom();

    // Create an event handler to trap any handlers being created for I2C services.
    // We do this to enable initialisation of those services only when they're used,
    // which saves processor time, memeory and battery life.
    messageBus.listen(DEVICE_ID_MESSAGE_BUS_LISTENER, DEVICE_EVT_ANY, this, &CM300::onListenerRegisteredEvent);

#if CONFIG_ENABLED(DMESG_SERIAL_DEBUG)
#if DEVICE_DMESG_BUFFER_SIZE > 0
    codal_dmesg_set_flush_fn(nano_dmesg_flush);
#endif
#endif

    status |= DEVICE_COMPONENT_STATUS_IDLE_TICK;

    return DEVICE_OK;
}

/**
  * A listener to perform actions as a result of Message Bus reflection.
  *
  * In some cases we want to perform lazy instantiation of components, such as
  * the compass and the accelerometer, where we only want to add them to the idle
  * fiber when someone has the intention of using these components.
  */
void CM300::onListenerRegisteredEvent(Event evt)
{
}

/**
  * A periodic callback invoked by the fiber scheduler idle thread.
  * We use this for any low priority, backgrounf housekeeping.
  *
  */
void CM300::idleCallback()
{
#if DEVICE_DMESG_BUFFER_SIZE > 0
    codal_dmesg_flush();
#endif
}

void nano_dmesg_flush()
{
#if CONFIG_ENABLED(DMESG_SERIAL_DEBUG)
#if DEVICE_DMESG_BUFFER_SIZE > 0
    // if (codalLogStore.ptr > 0 && cm300_device_instance)
    // {
    //     for (uint32_t i=0; i<codalLogStore.ptr; i++)
    //         ((CM300 *)cm300_device_instance)->serial.putc(codalLogStore.buffer[i]);

    //     codalLogStore.ptr = 0;
    // }
#endif
#endif
}
