#include "CM300.h"
#include "CM300SAADC.h"

CM300 cm;

/* Sample global code START */
NRF52Pin cds(ID_PIN_P0_29, P0_29, PIN_CAPABILITY_ALL);    // 29 SAADC5
NRF52Pin bat(ID_PIN_P0_31, P0_31, PIN_CAPABILITY_ALL);    // 31 SAADC7
NRF52Pin dis1(ID_PIN_P0_02, P0_2, PIN_CAPABILITY_ALL);   // 2  SAADC0
NRF52Pin dis2(ID_PIN_P0_30, P0_30, PIN_CAPABILITY_ALL);   // 30 SAADC6 
NRF52Pin dis3(ID_PIN_P0_28, P0_28, PIN_CAPABILITY_ALL);   // 28 SAADC4
NRF52Pin dis4(ID_PIN_P0_03, P0_3, PIN_CAPABILITY_ALL);   // 3  SAADC1

NRF52Pin batEna(ID_PIN_P0_00, P0_0, PIN_CAPABILITY_DIGITAL);

CM300SAADC adc0(dis1, DEVICE_ID_IO_P0);
CM300SAADC adc1(dis4, DEVICE_ID_IO_P0);
CM300SAADC adc4(dis3, DEVICE_ID_IO_P0);
CM300SAADC adc5(cds, DEVICE_ID_IO_P0);
CM300SAADC adc6(dis2, DEVICE_ID_IO_P0);
CM300SAADC adc7(bat, DEVICE_ID_IO_P0);
/* Sample global code END */

int main()
{
  cm.init();

  int16_t temp = 0;
  /* Sample init code START */
  
  /* Sample init code END */

  batEna.setDigitalValue(1);
  while(1)
  {
    /* Sample loop code START */
    temp = adc0.readValue();
    cm.serial.printf("\n ADC0 : %d ",temp);
    codal::system_timer_wait_ms(1);

    temp = adc1.readValue();
    cm.serial.printf("ADC1 : %d ",temp);
    codal::system_timer_wait_ms(1);
    
    temp = adc4.readValue();
    cm.serial.printf("ADC4 : %d ",temp);
    codal::system_timer_wait_ms(1);

    temp = adc5.readValue();
    cm.serial.printf("ADC5 : %d ",temp);
    codal::system_timer_wait_ms(1);

    temp = adc6.readValue();
    cm.serial.printf("ADC6 : %d ",temp);
    codal::system_timer_wait_ms(1);

    temp = adc7.readValue();
    cm.serial.printf("ADC7 : %d \t\n",temp);
    codal::system_timer_wait_ms(1);


    codal::system_timer_wait_ms(500);
    /* Sample loop code END */
  }
}
