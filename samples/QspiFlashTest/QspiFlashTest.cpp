#include "CM300.h"
CM300 cm;

/* Sample global code START */
#include "CM300QSPIFlash.h"
CM300QSPIFlash qspi_flash;
/* Sample global code END */

int main()
{
  cm.init();

  /* Sample init code START */
  uint16_t i;
  const uint16_t BUFFER_SIZE = 1024;
  uint8_t write_buffer[BUFFER_SIZE] = {0,};
  // the first 4 bytes of read_buffer are empty due to 1 byte
  // [read command] and 3 bytes [address]
  uint8_t read_buffer[BUFFER_SIZE] = {0,};
  cm.serial.printf("Start CM-300 SPI Flash test. \
    Buffer size = %d\r\n\r\n", BUFFER_SIZE);
  /* Sample init code END */

  while(1)
  {
    /* Sample loop code START */
    // Low activity
    if(cm.io.start_button.getDigitalValue() == false){
      for (i = 0; i < BUFFER_SIZE; i++)
      {
        write_buffer[i] = cm.systemTime() + i;
        read_buffer[i] = 0;
      }
      cm.serial.printf("Erase 4k block starting from address 0.\r\n");
      qspi_flash.erase(0, 4096);

      cm.serial.printf("Write [write data buffer] \
        to area starting from address 0.\r\n");
      qspi_flash.write(0, write_buffer, sizeof(write_buffer));

      cm.serial.printf("Read area starting \
        from address 0 and save to [read data buffer].\r\n");
      qspi_flash.read(0, read_buffer, sizeof(read_buffer));

      cm.serial.printf("\r\nPrint [write data buffer] \
        and [read data buffer].\r\nindex w  r\r\n");
      for (i = 0; i < sizeof(read_buffer); i++)
      {
        cm.serial.printf(" %d    %d %d\r\n", \
          i, write_buffer[i], read_buffer[i]);
        cm.sleep(1);
        // Blink the LED to check operation.
        cm.blinkLed(500);          
      }
    }
    /* Sample loop code END */

    // Blink the LED to check operation.
    cm.blinkLed(500);    
  }
}
