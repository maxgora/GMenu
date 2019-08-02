#include "screen_mng.h"
#include "header.h"

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

gFont font1; // малый шрифт - UI2 (h=8pxls)
gFont font2; // средний шрифт - times12bw (с кириллицей, h=12pxls)
gFont font3; // крупный шрифт - fixed_10x20 (для времени)

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

uint8_t active_screen = 0;
uint8_t old_screen = 0;
                                          
void ScreenPool(ButtonPushType btn)
{
  ScreenReturnType ret;
  
  ret = Screen[active_screen].pScreenPool(btn);
  if ((ret != active_screen) & (ret != SCREENx_RET_OK ) )
  {
    if (ret < SCREENx_RET_OK)
    { // запрос на переход к другому экрану
      old_screen = active_screen;
      active_screen = ret;
      Screen[active_screen].pScreenInit(White, Black);
    }
    else if (ret == SCREENx_RETURN)
    {
      active_screen = old_screen;
      Screen[active_screen].pScreenInit(White, Black);
    }
  }
}

void ScreenInit()
{
  gdispImage img;
  //  gdispImageError err;
  
  // вывод логотипа
  /*err = */gdispImageOpenFile(&img, "logo.bmp");
  /*err = */gdispImageDraw(&img, 16, 0, 128, 128, 0, 0);
  gdispImageClose(&img);    
  LCD_LED_ON();
  
  //osDelay(1500);
  gdispClear(White);  // очистка экрана
  
  font1 = gdispOpenFont("UI2");
  font2 = gdispOpenFont("times12bw");//("SCPR12bw");//("arialn12bw");//("lucon12bw");//
  font3 = gdispOpenFont("fixed_10x20");
  
  HeaderInit(White, Black);
  
  active_screen = 0;
  Screen[active_screen].pScreenInit(White, Black);
}

