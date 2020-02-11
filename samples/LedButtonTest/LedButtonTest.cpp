#include "CM300.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

static void button_led_task_function(void * pvParameter);

CM300 cm;

int main()
{
  cm.init();

  /* Create task for Button-LED with priority set to 2 */
  xTaskCreate(button_led_task_function, "BUTTON-LED", configMINIMAL_STACK_SIZE + 200, NULL, 2, NULL);

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
static void button_led_task_function(void * pvParameter)
{
  (void)pvParameter;
  bool led_state;
  
  while(true){
    led_state = cm.io.start_button.getDigitalValue();
    cm.io.led.setDigitalValue(led_state);

    /* Delay a task for a given number of ticks */
    vTaskDelay(100);

    /* Tasks must be implemented to never return... */
  }
}