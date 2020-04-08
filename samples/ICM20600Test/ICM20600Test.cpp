#include "CM300.h"

CM300 cm;

/* Sample global code START */
#include "ICM20600.h"
#include "NRF52I2C.h"
#include "device_pinmap.h"
/* Sample global code END */

int main()
{
  cm.init();

  /* Sample init code START */
  int16_t x = 0, y = 0, z = 0;

  NRF52Pin sda (ID_PIN_P0_11, P0_11, PIN_CAPABILITY_DIGITAL);
  NRF52Pin scl (ID_PIN_P1_09, P1_9, PIN_CAPABILITY_DIGITAL);
  NRF52Pin int1 (ID_PIN_P0_05, P0_5, PIN_CAPABILITY_DIGITAL);
  NRF52I2C i2c (sda, scl);
  CoordinateSpace space(SIMPLE_CARTESIAN, false, COORDINATE_SPACE_ROTATED_0);

  ICM20600 imu( i2c, int1, space);
  imu.configure();
  /* Sample init code END */

  while(1)
  {
    x = imu.getX();
    y = imu.getY();
    z = imu.getZ();
    cm.serial.printf(" Ax : %d, Ay : %d, Az : %d \r\n", x, y, z );

    x = imu.getRoll(); 
    y = imu.getPitch();
    z = 0;
    cm.serial.printf(" Roll : %d, Pitch : %d, YAW : %d \r\n\n\n", x, y, z );
     
    cm.sleep(500);
    // codal::system_timer_wait_ms(500);
    cm.blinkLed(500);
  }
}
