#include "CM300.h"
CM300 cm;

/* Sample global code START */
#include "CM300NUS.h"
/* Sample global code END */

int main()
{
  cm.init();

  /* Sample init code START */
  int pre_s_btn_state = cm.io.start_button.getDigitalValue();
  int s_btn_state;

  uint32_t notify_cnt = 0;
  bleUartInit();
  /* Sample init code END */

  while(1)
  {
    /* Sample loop code START */
    bleUartUpdate();

    while(bleUartAvailable()){
      cm.serial.putc((char)bleUartRead());
    }

    s_btn_state = cm.io.start_button.getDigitalValue();
    if(s_btn_state && (pre_s_btn_state != s_btn_state))
    {
      bleUartPrintf("Hello, CM300 : %d", ++notify_cnt);
    }
    pre_s_btn_state = s_btn_state;
    /* Sample loop code END */

    // Blink the LED to check operation.
    cm.blinkLed(500);
  }
}
