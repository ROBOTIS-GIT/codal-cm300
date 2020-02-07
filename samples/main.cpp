#include "CM300.h"

CM300 cm;

int main()
{
    bool led_state;

    cm.init();

    while(1)
    {
        led_state = cm.io.start_button.getDigitalValue();
        cm.io.led.setDigitalValue(led_state);
    }
}