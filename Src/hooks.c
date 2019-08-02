#include "hooks.h"
#include "gfx.h"
#include "screen_mng.h"
#include "utils.h"


extern font_t font1; // малый шрифт - UI2 (h=8pxls)
extern font_t font2; // средний шрифт - times12bw (с кириллицей, h=12pxls)
extern font_t font3; // крупный шрифт - fixed_10x20 (для времени)

/*
Сообщение, после перехвата ошибки при переполнении стека
*/
void vApplicationStackOverflowHook( xTaskHandle *pxTsk, signed portCHAR *pcTskNm )
{
  taskDISABLE_INTERRUPTS();
  
  font_t font = font1;
  gCoord sh = gdispGetFontMetric(font, gFontHeight); // string hegth
  Area a = {{0,0},{160,128}};
  //gColor c = White;
  
  gdispFillArea(a.p.x, a.p.y, a.size.x, a.size.y, Black);
  
  gdispDrawString(a.p.x, a.p.y, "Error: StackOverflow.", font, White);  
  
  a.p.y += (sh + 2);
  gdispDrawString(a.p.x + 15, a.p.y, "Task Name: ", font, White);
  
  a.p.y += (sh + 2);
  gdispDrawString(a.p.x + 25, a.p.y, (char*)pcTskNm, font, White);

  while(1);
}

/*
Сообщение, после перехвата ошибки при выделении памяти из кучи
*/
void vApplicationMallocFailedHook( void )
{
  taskDISABLE_INTERRUPTS();
  
  font_t font = font1;
  gCoord sh = gdispGetFontMetric(font, gFontHeight); // string hegth
  Area a = {{0,0},{160,128}};

  gdispFillArea(a.p.x, a.p.y, a.size.x, a.size.y, Black);
  
  gdispDrawString(a.p.x, a.p.y, "Error: MallocFailed", font, White);  

  while(1);
}

/*
Сообщение об ошибке, вызываемое FreeRTOS
*/
void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
  taskDISABLE_INTERRUPTS(); 

  font_t font = font1;
  gCoord sh = gdispGetFontMetric(font, gFontHeight); // string hegth
  Area a = {{0,0},{160,128}};
  char buff[11];
  char *str;

  gdispFillArea(a.p.x, a.p.y, a.size.x, a.size.y, Black);
  
  gdispDrawString(a.p.x, a.p.y, "Error: AssertCalled.", font, White);

  a.p.y += (sh + 2);
  gdispDrawString(a.p.x + 15, a.p.y,   "File Name: ", font, White);
  
  a.p.y += (sh + 2);
  gdispDrawString(a.p.x + 25, a.p.y, pcFileName, font, White);
  
  a.p.y += (sh + 2);
  gdispDrawString(a.p.x + 15, a.p.y, "Line: ", font, White);
  
  str = utoa_fast_div(ulLine, buff);
  a.p.y += (sh + 2);
  gdispDrawString(a.p.x + 25, a.p.y, str, font, White);

  while(1);
}

/*
*/
void vMyFatalErrAssert(char *file, uint32_t line)
{
  taskDISABLE_INTERRUPTS(); 

  font_t font = font1;
  gCoord sh = gdispGetFontMetric(font, gFontHeight); // string hegth
  Area a = {{0,0},{160,128}};
  char buff[11];
  char *str;

  gdispFillArea(a.p.x, a.p.y, a.size.x, a.size.y, Black);
  
  gdispDrawString(a.p.x, a.p.y, "Wrong parameters value: file ", font, White);
  
  a.p.y += (sh + 2);
  gdispDrawString(a.p.x + 15, a.p.y, file, font, White);
  
  a.p.y += (sh + 2);
  gdispDrawString(a.p.x + 15, a.p.y, "on line ", font, White);
  
  str = utoa_fast_div(line, buff);
  a.p.y += (sh + 2);
  gdispDrawString(a.p.x + 15, a.p.y, str, font, White);

  while(1);
}
