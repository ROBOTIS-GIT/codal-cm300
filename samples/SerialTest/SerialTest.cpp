#include "CM300.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

static void serial_task_function(void * pvParameter);

CM300 cm;

int main()
{
  cm.init();

  /* Create task for Button-LED with priority set to 2 */
  xTaskCreate(serial_task_function, "SERIAL", configMINIMAL_STACK_SIZE + 200, NULL, 2, NULL);

  /* Start FreeRTOS scheduler. */
  vTaskStartScheduler();

  while(1)
  {
  }
}

/**@brief Button-LED task entry function.
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the task.
 */
static void serial_task_function(void * pvParameter)
{
  (void)pvParameter;
  bool led_state = false;
  uint32_t count = 0;
  
  while(true){
    cm.io.led.setDigitalValue(led_state);
    led_state = 1 - led_state;
    cm.serial.uart.putc('!');
    cm.serial.uart.printf(" %d %s", count++, "Serial test\r\n");

    /* Delay a task for a given number of ticks */
    vTaskDelay(500);

    /* Tasks must be implemented to never return... */
  }
}
