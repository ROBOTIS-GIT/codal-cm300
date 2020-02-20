#include "CM300.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "CM300Serial.h"

static void led_blink_task_function(void * pvParameter);
static void serial_loopback_task_function(void * pvParameter);

CM300 cm;

int main()
{
  cm.init();

  /* Create task for LED with priority set to 2 */
  xTaskCreate(led_blink_task_function, \
    "LED", configMINIMAL_STACK_SIZE + 200, NULL, 2, NULL);

  /* Create task for Serial with priority set to 2 */
  xTaskCreate(serial_loopback_task_function, \
    "SERIAL LOOPBACK", 2*1024, NULL, 2, NULL);

  /* Start FreeRTOS scheduler. */
  vTaskStartScheduler();

  while(1)
  {
  }
}

const uint8_t TX_BUF_SIZE = 32;
const uint8_t RX_BUF_SIZE = 32;
static void serial_loopback_task_function(void * pvParameter)
{
  (void)pvParameter;
  CM300Serial serial;
  uint8_t rx_buf[RX_BUF_SIZE];
  int rx_len;
  
  serial.uart.setBaud(1000000);
  serial.uart.setTxBufferSize(TX_BUF_SIZE);
  serial.uart.setRxBufferSize(RX_BUF_SIZE);

  for(;;){
    rx_len = serial.uart.rxBufferedSize();
    if(rx_len > 0){
      if(rx_len > RX_BUF_SIZE){
        rx_len = RX_BUF_SIZE;
      }
      rx_len = serial.uart.read(rx_buf, rx_len, SYNC_SPINWAIT);
      serial.uart.send(rx_buf, rx_len, ASYNC);
    }

    /* Tasks must be implemented to never return... */
  }
}

/**@brief LED task entry function.
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the task.
 */
static void led_blink_task_function(void * pvParameter)
{
  (void)pvParameter;
  
  for(;;){
    cm.io.led.setDigitalValue(!cm.io.led.getDigitalValue());

    /* Delay a task for a given number of ticks */
    vTaskDelay(500);

    /* Tasks must be implemented to never return... */
  }
}