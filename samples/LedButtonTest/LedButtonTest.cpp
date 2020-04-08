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
  bool led_state = false;
  cm.io.led.setDigitalValue(led_state);
  /* Sample init code END */

  while(1)
  {
    /* Sample loop code START */
    led_state = cm.io.start_button.getDigitalValue();
    cm.io.led.setDigitalValue(led_state);   
    cm.sleep(100);
    /* Sample loop code END */
  }
}
