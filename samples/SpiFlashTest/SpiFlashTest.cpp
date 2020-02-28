#include "CM300.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "CM300Serial.h"
#include "CM300SPI_Flash.h"

static void led_blink_task_function(void * pvParameter);
static void spi_flash_task_function(void * pvParameter);

#define BUFFER_SIZE 1024

CM300 cm;
CM300Serial serial;
CM300SPI_Flash spi_flash;

uint8_t write_buffer[BUFFER_SIZE] = {0,};
// the first 4 bytes of read_buffer are empty due to 1 byte [read command] and 3 bytes [address]
uint8_t read_buffer[BUFFER_SIZE + 4] = {0,};

int main()
{
  cm.init();

  /* Create task for LED with priority set to 2 */
  xTaskCreate(led_blink_task_function, "LED", configMINIMAL_STACK_SIZE + 200, NULL, 2, NULL);

  /* Create task for SPI Flash with priority set to 2 */
  xTaskCreate(spi_flash_task_function, "SPI_Flash", configMINIMAL_STACK_SIZE + 200, NULL, 2, NULL);

  /* Start FreeRTOS scheduler. */
  vTaskStartScheduler();

  while(1)
  {
  }
}


static void spi_flash_task_function(void * pvParameter)
{
  (void)pvParameter;

  serial.uart.setBaud(1000000);

  spi_flash.writeDisable();

  serial.uart.printf("Start CM-300 SPI Flash test. Buffer size = %d\r\n\r\n", BUFFER_SIZE);

  for(;;){
    // wait for button press and release
    while(cm.io.mode_button.getDigitalValue());
    while(!cm.io.mode_button.getDigitalValue());

    uint32_t i = 0;
    for (i = 0; i < BUFFER_SIZE; i++)
    {
      write_buffer[i] = cm.systemTime() + i;
      read_buffer[i] = 0;
    }
    serial.uart.printf("Erase 4k block starting from address 0.\r\n");
    spi_flash.eraseSmallRow(0);

    serial.uart.printf("Write [write data buffer] to area starting from address 0.\r\n");
    spi_flash.writeBytes(0, write_buffer, sizeof(write_buffer));

    serial.uart.printf("Read area starting from address 0 and save to [read data buffer].\r\n");
    spi_flash.readBytes(0, read_buffer, sizeof(read_buffer));

    serial.uart.printf("\r\nPrint [write data buffer] and [read data buffer].\r\nindex w  r\r\n");
    for (i = 0; i < sizeof(read_buffer) - 4; i++)
    {
      serial.uart.printf(" %d    %d %d\r\n", i, write_buffer[i], read_buffer[4+i]);

      vTaskDelay(1);
    }

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
