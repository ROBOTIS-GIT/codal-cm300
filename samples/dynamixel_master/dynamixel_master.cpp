#include "CM300.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "CM300DYNAMIXEL.h"

static void led_blink_task_function(void * pvParameter);
static void dxl_master_task_function(void * pvParameter);

CM300 cm;
CM300DYNAMIXEL dynamixel;

int main()
{
  cm.init();

  /* Create task for LED with priority set to 2 */
  xTaskCreate(led_blink_task_function, "LED", configMINIMAL_STACK_SIZE + 200, NULL, 2, NULL);

  /* Create task for DYNAMIXEL Master with priority set to 2 */
  xTaskCreate(dxl_master_task_function, "DXL Master", 2*1024, NULL, 2, NULL);

  /* Start FreeRTOS scheduler. */
  vTaskStartScheduler();

  while(1)
  {
  }
}


static void dxl_master_task_function(void * pvParameter)
{
  (void)pvParameter;
  uint8_t id;
  
  for(;;){
    dynamixel.ping(1, &id, 1);
    vTaskDelay(500);
  }
}


/**@brief LED task entry function.
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the task.
 */
static void led_blink_task_function(void * pvParameter)
{
  (void)pvParameter;
  
  while(true){
    cm.io.led.setDigitalValue(!cm.io.led.getDigitalValue());

    /* Delay a task for a given number of ticks */
    vTaskDelay(500);

    /* Tasks must be implemented to never return... */
  }
}