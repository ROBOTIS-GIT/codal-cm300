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

#ifndef CM300_H
#define CM300_H

#include "CodalHeapAllocator.h"
#include "codal-core/inc/types/Event.h"
#include "CodalDevice.h"
#include "ErrorNo.h"
#include "NRFLowLevelTimer.h"
#include "CodalCompat.h"
#include "CodalComponent.h"
#include "CodalDmesg.h"
#include "ManagedType.h"
#include "ManagedString.h"
#include "NotifyEvents.h"

#include "Button.h"
#include "Timer.h"

#include "CodalFiber.h"
#include "MessageBus.h"

#include "CM300IO.h"
#include "CM300Serial.h"

// Status flag values
#define DEVICE_INITIALIZED                    0x01

/**
 * Class definition for a CM300 device.
 *
 * Represents the device as a whole, and includes member variables that represent various device drivers
 * used to control aspects of the CM300.
 */
namespace codal
{
    class CM300 : public CodalDevice, public CodalComponent
    {
        private:
            /**
             * A listener to perform actions as a result of Message Bus reflection.
             *
             * In some cases we want to perform lazy instantiation of components, such as
             * the compass and the accelerometer, where we only want to add them to the idle
             * fiber when someone has the intention of using these components.
             */
            void onListenerRegisteredEvent(Event evt);

        public:
            NRFLowLevelTimer    timer1;
            Timer               timer;
            MessageBus          messageBus;

            CM300IO             io;
            CM300Serial         serial;

            /**
             * Constructor.
             *
             * Create a representation of a Genuino Zero device, which includes member variables
             * that represent various device drivers used to control aspects of the board.
             */
            CM300();

            /**
             * Post constructor initialisation method.
             */
            int init();

            /**
             * Delay execution for the given amount of time.
             *
             * If the scheduler is running, this will deschedule the current fiber and perform
             * a power efficient, concurrent sleep operation.
             *
             * If the scheduler is disabled or we're running in an interrupt context, this
             * will revert to a busy wait.
             *
             * Alternatively: wait, wait_ms, wait_us can be used which will perform a blocking sleep
             * operation.
             *
             * @param milliseconds the amount of time, in ms, to wait for. This number cannot be negative.
             *
             * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER milliseconds is less than zero.
             *
             */
            void sleep(uint32_t milliseconds);

            /**
             * A periodic callback invoked by the fiber scheduler idle thread.
             * We use this for any low priority, background housekeeping.
             */
            virtual void idleCallback();

            /**
             * Determine the time since this CM300 was last reset.
             *
             * @return The time since the last reset, in milliseconds.
             *
             * @note This will value overflow after 1.6 months.
             */
            //TODO: handle overflow case.
            unsigned long systemTime();

            // Function to check operation using LED.
            void blinkLed(uint32_t interval_ms);
    };


    /**
     * Delay execution for the given amount of time.
     *
     * If the scheduler is running, this will deschedule the current fiber and perform
     * a power efficient, concurrent sleep operation.
     *
     * If the scheduler is disabled or we're running in an interrupt context, this
     * will revert to a busy wait.
     *
     * Alternatively: wait, wait_ms, wait_us can be used which will perform a blocking sleep
     * operation.
     *
     * @param milliseconds the amount of time, in ms, to wait for. This number cannot be negative.
     *
     * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER milliseconds is less than zero.
     *
     */
    inline void CM300::sleep(uint32_t milliseconds)
    {
        fiber_sleep(milliseconds);
    }

    /**
     * Determine the time since this CM300 was last reset.
     *
     * @return The time since the last reset, in milliseconds.
     *
     * @note This will value overflow after 1.6 months.
     */
    inline unsigned long CM300::systemTime()
    {
        return system_timer_current_time();
    }
}

void nano_dmesg_flush();

using namespace codal;

#endif
