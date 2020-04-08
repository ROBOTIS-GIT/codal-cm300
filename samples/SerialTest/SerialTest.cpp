#include "CM300.h"
CM300 cm;

/* Sample global code START */
  // Headers for sample code
  // Global Class instances
  // Global variables
  // Global etc.
/* Sample global code END */

int main()
{
  cm.init();

  /* Sample init code START */
  const uint8_t TX_BUF_SIZE = 32;
  const uint8_t RX_BUF_SIZE = 32;  
  uint8_t rx_buf[RX_BUF_SIZE];
  int rx_len;
  cm.serial.setBaud(1000000);
  cm.serial.setTxBufferSize(TX_BUF_SIZE);
  cm.serial.setRxBufferSize(RX_BUF_SIZE);
  /* Sample init code END */

  while(1)
  {
    /* Sample loop code START */
    rx_len = cm.serial.rxBufferedSize();
    if(rx_len > 0){
      if(rx_len > RX_BUF_SIZE){
        rx_len = RX_BUF_SIZE;
      }
      rx_len = cm.serial.read(rx_buf, rx_len, SYNC_SLEEP);
      cm.serial.send(rx_buf, rx_len, SYNC_SLEEP);
    }
    /* Sample loop code END */

    // Blink the LED to check operation.
    cm.blinkLed(500);
  }
}

