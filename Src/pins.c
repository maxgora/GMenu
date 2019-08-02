#include <stdint.h>

#include "pins.h"
#include "val.h"

void LedsOff(void)
{
  LED_GREEN_OFF();
  LED_RED_OFF();
}

void LedGreenToggle(void)
{
  LED_GREEN_SWITCH();
}
