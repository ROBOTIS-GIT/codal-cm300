#include "CM300.h"
CM300 cm;

/* Sample global code START */
#include "CM300DYNAMIXEL.h"
CM300DYNAMIXEL dynamixel;
/* Sample global code END */

int main()
{
  cm.init();

  /* Sample init code START */
  uint8_t id[253];
  uint8_t recv_cnt, i;
  uint32_t pre_time = 0;
  /* Sample init code END */

  while(1)
  {
    /* Sample loop code START */
    if(cm.systemTime() - pre_time >= 500){
      pre_time = cm.systemTime();
      recv_cnt = dynamixel.ping(DXL_BROADCAST_ID, id, sizeof(id));
      cm.serial.printf("Ping recv cnt: %d \r\n", recv_cnt);
      for(i=0; i<recv_cnt; i++)
      {
        cm.serial.printf("\t ID: %d\r\n", id[i]);
      }
    }
    /* Sample loop code END */

    // Blink the LED to check operation.
    cm.blinkLed(500);
  }
}
