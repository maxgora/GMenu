#include "screen_mng.h"
#include "header.h"
#include "hooks.h"

#include "screen0.h"
#include "screen1.h"
#include "screen2.h"
#include "screen3.h"
#include "screen4.h"
#include "screen5.h"
#include "screen6.h"
#include "screen7.h"
#include "screen8.h"
#include "screen9.h"
#include "screen10.h"
#include "screen11.h"

extern GlobValType GV;

gFont font1; // малый шрифт - UI2 (h=8pxls)
gFont font2; // средний шрифт - times12bw (с кириллицей, h=12pxls)
gFont font3; // крупный шрифт - (для времени)

#define SCREEN_CNT_MAX  12

// Массив функций обработки интерфейса
ScreenHandlersType Screen[ SCREEN_CNT_MAX ] = {
  {"Start", Screen0Init, Screen0Pool},
  {"Settings view", Screen1Init, Screen1Pool},
  {"Measure", Screen2Init, Screen2Pool},
  {"Operations during measure", Screen3Init, Screen3Pool},
  {"Question about stop", Screen4Init, Screen4Pool},
  {"ESG view", Screen5Init, Screen5Pool},
  {"Settings change", Screen6Init, Screen6Pool},
  {"Name change", Screen7Init, Screen7Pool},
  {"Mode change", Screen8Init, Screen8Pool},
  {"Measure Time change", Screen9Init, Screen9Pool},
  {"Frequeny change", Screen10Init, Screen10Pool},
  {"Pacemaker control", Screen11Init, Screen11Pool}
};

//uint8_t active_screen = 0;
//uint8_t old_screen = 0;

// стек активности экранов                          
QueueHandle_t xQueueScreen;
                                          
void ScreenPool(ButtonPushType btn)
{ 
  ScreenReturnType ret;
  uint8_t active_screen;
  BaseType_t rtos_ret;
  
  // получение активного экрана из очереди без
  // удаления из очереди
  rtos_ret = xQueuePeek(xQueueScreen, &active_screen, 0 ); 
  MY_CHECK(rtos_ret);
  
  rtos_ret = uxQueueSpacesAvailable(xQueueScreen);
  
  ret = Screen[active_screen].pScreenPool(btn);
  if ((ret != active_screen) & (ret != SCREENx_RET_OK ) )
  {// смена смена активного экрана
    
    if (ret == SCREENx_RET_TO_SCREEN0)
    { // переход с любого экрана к экрану 0
      // должен приводить в ИС стек активности экрана
      active_screen = 0;
      rtos_ret = xQueueReset(xQueueScreen);
      MY_CHECK(rtos_ret);
      rtos_ret = xQueueSendToFront(xQueueScreen, &active_screen, 0 );
      MY_CHECK(rtos_ret);
      // ини экрана 0
      Screen[active_screen].pScreenInit(White, Black);      
    }
    else if (ret < SCREENx_RET_OK)
    { // запрос на переход к другому экрану
      active_screen = ret;
      // поместить в стек следующий активный экран
      rtos_ret = xQueueSendToFront(xQueueScreen, &active_screen, 0 );  
      MY_CHECK(rtos_ret);
      
      rtos_ret = uxQueueSpacesAvailable(xQueueScreen);
      
      // ини активного экрана
      Screen[active_screen].pScreenInit(White, Black);
    }
    else if (ret == SCREENx_RETURN)
    {
      // извлечение из стека активного
      rtos_ret = xQueueReceive(xQueueScreen, &active_screen, 0 );
      MY_CHECK(rtos_ret);
      // получение предыдущего активного экрана
      rtos_ret = xQueuePeek(xQueueScreen, &active_screen, 0 );
      MY_CHECK(rtos_ret);
      
      rtos_ret = uxQueueSpacesAvailable(xQueueScreen);
      
      // ини активного экрана
      Screen[active_screen].pScreenInit(White, Black);
    }
  }
}

void ScreenInit()
{
  uint8_t active_screen = 0;
  BaseType_t rtos_ret;
  
  gdispImage img;
  //  gdispImageError err;
  
  // вывод логотипа
  /*err = */gdispImageOpenFile(&img, "logo.bmp");
  /*err = */gdispImageDraw(&img, 16, 0, 128, 128, 0, 0);
  gdispImageClose(&img);    
  LCD_LED_ON();
  
  osDelay(1500);
  gdispClear(White);  // очистка экрана
  
  font1 = gdispOpenFont("UI2");
  font2 = gdispOpenFont("times12bw");//("SCPR12bw");//("arialn12bw");//("lucon12bw");//
  font3 = gdispOpenFont("DejaVuSans20");//DejaVuSans24");//fixed_10x20");
  
  HeaderInit(White, Black);
  
  xQueueScreen = xQueueCreate( 16, sizeof( active_screen ) );
  MY_CHECK(xQueueScreen);
  
  rtos_ret = xQueueSendToFront(xQueueScreen, &active_screen, 0 );  // размещение в стек первый активный экран
  MY_CHECK(rtos_ret);
  
  Screen[active_screen].pScreenInit(White, Black);
}

void BluetoothSwitch( void* pval )
{
  GV.bluetooth_state = !GV.bluetooth_state;
  HeaderBtUpdate(GV.bluetooth_state);
}
