#include "CM300.h"
CM300 cm;

/* Sample global code START */
#include "CM300SpeakerMIC.h"
CM300SpeakerMIC mic_spk; 
/* Sample global code END */

int main()
{
  cm.init();

  /* Sample init code START */
  int pre_s_btn_state = cm.io.start_button.getDigitalValue();
  int pre_m_btn_state = cm.io.mode_button.getDigitalValue();
  int s_btn_state;
  int m_btn_state;
  /* Sample init code END */

  while(1)
  {
    /* Sample loop code START */
    s_btn_state = cm.io.start_button.getDigitalValue();
    if(s_btn_state && (pre_s_btn_state != s_btn_state))
    {
      mic_spk.play(0, false);
      // cm.serial.printf("I2S play frequency: %d - %d (hz)\r\n", mic_spk.desiredSampleRate(), mic_spk.sampleRate());
      // cm.serial.printf("\r\n");
    }
    pre_s_btn_state = s_btn_state;

    m_btn_state = cm.io.mode_button.getDigitalValue();
    if(m_btn_state && (pre_m_btn_state != m_btn_state))
    {
      mic_spk.record(0);
      // cm.serial.printf("I2S record frequency: %d - %d (hz)\r\n", mic_spk.desiredSampleRate(), mic_spk.sampleRate());
      // cm.serial.printf("\r\n");
    }
    pre_m_btn_state = m_btn_state;
    /* Sample loop code END */

    // Blink the LED to check operation.
    cm.blinkLed(500);
  }
}
