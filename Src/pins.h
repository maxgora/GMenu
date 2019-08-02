#ifndef __PINS_H
#define __PINS_H

#include "main.h"

#define LED_RED_ON()        HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_SET)
#define LED_RED_OFF()       HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,GPIO_PIN_RESET)
#define LED_RED_SWITCH()    HAL_GPIO_TogglePin(LED_RED_GPIO_Port,LED_RED_Pin)
#define bLED_RED_ON()       (HAL_GPIO_ReadPin(LED_RED_GPIO_Port,LED_RED_Pin) == GPIO_PIN_SET)

#define LED_GREEN_ON()      HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_SET)
#define LED_GREEN_OFF()     HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_RESET)
#define LED_GREEN_SWITCH()  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin)
#define bLED_GREEN_ON()     (HAL_GPIO_ReadPin(LED_GREEN_GPIO_Port,LED_GREEN_Pin) == GPIO_PIN_SET)

#define bBTN_UP()           (HAL_GPIO_ReadPin(nBTN_UP_GPIO_Port,nBTN_UP_Pin) == GPIO_PIN_RESET)
#define bBTN_SET()          (HAL_GPIO_ReadPin(nBTN_SET_GPIO_Port,nBTN_SET_Pin) == GPIO_PIN_RESET)
#define bBTN_DOWN()         (HAL_GPIO_ReadPin(nBTN_DOWN_GPIO_Port,nBTN_DOWN_Pin) == GPIO_PIN_RESET)
#define bBTN_USER()         (HAL_GPIO_ReadPin(nBTN_USER_GPIO_Port,nBTN_USER_Pin) == GPIO_PIN_RESET)

#define LCD_ON()            HAL_GPIO_WritePin(nLCD_ON_GPIO_Port,nLCD_ON_Pin,GPIO_PIN_RESET)
#define LCD_OFF()           HAL_GPIO_WritePin(nLCD_ON_GPIO_Port,nLCD_ON_Pin,GPIO_PIN_SET)

#define LCD_LED_ON()        HAL_GPIO_WritePin(nLCD_LED_GPIO_Port,nLCD_LED_Pin,GPIO_PIN_RESET)
#define LCD_LED_OFF()       HAL_GPIO_WritePin(nLCD_LED_GPIO_Port,nLCD_LED_Pin,GPIO_PIN_SET)
#define LCD_LED_SWITCH()    HAL_GPIO_TogglePin(nLCD_LED_GPIO_Port,nLCD_LED_Pin)

#define LCD_RST_ON()        HAL_GPIO_WritePin(nLCD_RST_GPIO_Port,nLCD_RST_Pin,GPIO_PIN_RESET)
#define LCD_RST_OFF()       HAL_GPIO_WritePin(nLCD_RST_GPIO_Port,nLCD_RST_Pin,GPIO_PIN_SET)

/*typedef enum {
  SIG_OFF = -1,
  SIG_UNDEF,
  SIG_ON
} SIGNAL_STATE;*/

void LedsOff(void);
void PinsPool_10ms(void);

#endif
