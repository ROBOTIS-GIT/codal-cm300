#include "CM300.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "CM300Serial.h"

static void led_blink_task_function(void * pvParameter);
static void serial_task_function(void * pvParameter);

CM300 cm;
CM300Serial serial;

int main()
{
  cm.init();

  /* Create task for LED with priority set to 2 */
  xTaskCreate(led_blink_task_function, "LED", configMINIMAL_STACK_SIZE + 200, NULL, 2, NULL);

  /* Create task for Serial with priority set to 2 */
  xTaskCreate(serial_task_function, "SERIAL", configMINIMAL_STACK_SIZE + 200, NULL, 2, NULL);

  /* Start FreeRTOS scheduler. */
  vTaskStartScheduler();

  while(1)
  {
  }
}


static void serial_task_function(void * pvParameter)
{
  (void)pvParameter;
  bool led_state = false;
  uint32_t count = 0;
  
  for(;;){
    cm.io.led.setDigitalValue(led_state);
    led_state = 1 - led_state;
    serial.uart.putc('!');
    serial.uart.printf(" %d %s", count++, "Serial test\r\n");

    /* Delay a task for a given number of ticks */
    vTaskDelay(500);

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