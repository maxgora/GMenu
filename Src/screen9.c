/*
Экран изменения длительности обследования
*/

#include <string.h>
#include "screen9.h"
#include "utils.h"
#include "DrawIco.h"
#include "hooks.h"

extern GlobValType GV;

extern gFont font1; // малый шрифт - UI2 (h=8pxls)
extern gFont font2; // средний шрифт - times12bw (с кириллицей, h=12pxls)
extern gFont font3; // крупный шрифт - (для времени)

static font_t font;

static const Area screen_area = {{0,16},{160,112}}; // активная область экрана {start point, size}

static gColor bgcolor = White;        // цвет фона
static gColor color = Black;          //

static const gPoint ico_up = {146,26};
static const gPoint ico_enter = {140,66};
static const gPoint ico_down = {146,106};

typedef enum  {day, hour, min} type_time;
type_time time_set;   // изменяемое время
static int8_t d,h,m; // время измерения

// очередь сообщений для задачи мигания цифр                            
static QueueHandle_t xQueueCursor;
//                                  
static TaskHandle_t xHandleNumberBlink = NULL;

/*
*/
static void Int8ToStr(int8_t n, char *dst)
{
  if (n == 0)
    dst[0] = dst[1] = '0';
  else if (n < 10)
  {
    dst[0] = '0';
    dst[1] = '0' + n;
  }
  else if (n < 99)
  {
    uint8_t m = n/10;
    dst[0] = '0' + m;
    dst[1] = '0' + (n - m*10);
  }
}

/*
Задача вывода на экран мигающих цифр
*/                                  
static void taskNumberBlink(void * argument)
{
  type_time time_set = day;
  type_time time_set_new;
  BaseType_t waite_res;
  gColor color_txt = color;
  gCoord fh = FONT_CH_H;
  gCoord fw_number = gdispGetStringWidth("0", font);
  gCoord fw_dot = gdispGetStringWidth(":", font);
  gPoint ref_poin, p;
  int8_t *ptime;
  char ch_buff[9] = {0};
  
  ref_poin.x = (ico_enter.x - screen_area.p.x - fw_number*6 - fw_dot*2)/2;
  if (ref_poin.x < 0)
    ref_poin.x = 5;
  ref_poin.y = screen_area.size.y / 2;

  p.x = ref_poin.x;
  p.y = ref_poin.y;
  ptime = &d;
  
  // формирование полной строки
  // дни
  Int8ToStr(d, &ch_buff[0]);
  ch_buff[2] = ':';
  // часы
  Int8ToStr(h, &ch_buff[3]);
  ch_buff[5] = ':';
  // минуты
  Int8ToStr(m, &ch_buff[6]);
  // вывод полной строки 
  gdispDrawStringBox(ref_poin.x, ref_poin.y, (fw_number*6 + fw_dot*2 + 2), fh,
    ch_buff, font, color, gJustifyLeft | gJustifyTop);
  // далее ch_buff исп-ся для преобразования отдельных позиций
  ch_buff[2] = 0; // конец строки
    
  while(1)
  {
    waite_res = xQueueReceive( xQueueCursor, &time_set_new, 500/portTICK_PERIOD_MS);
    
    // управление цветом выводимого текста
    if (waite_res == pdTRUE)
      color_txt = color;
    else if (color_txt == bgcolor)
      color_txt = color;
    else// if (color_txt == color)
      color_txt = bgcolor;
    
    // получение строки из цифр
    Int8ToStr(*ptime, ch_buff);
    // вывод строки в соотв позицию
    gdispFillStringBox(p.x, p.y, fw_number*2 + 2, fh+1,
      ch_buff, font, color_txt, bgcolor, gJustifyLeft | gJustifyTop);
    
    if (waite_res == pdTRUE)
    { // смена позиции
      time_set = time_set_new;
      
      if (time_set == day)
      {
        p.x = ref_poin.x;
        ptime = &d; 
      }
      else if (time_set == hour)
      {
        p.x = ref_poin.x + fw_number*2 + fw_dot;
        ptime = &h;
      }
      else
      {
        p.x = ref_poin.x + 2*(fw_number*2 + fw_dot);
        ptime = &m;
      }
    } // end if (waite_res == pdTRUE)  
  }
}


static void DeInitScreen()
{
  // освобождение памяти
  
  vTaskDelete( xHandleNumberBlink );
  xHandleNumberBlink = NULL;
  vQueueDelete( xQueueCursor );
  xQueueCursor = NULL;
}
                        
// обработка событий экрана
ScreenReturnType Screen9Pool(ButtonPushType btn)
{
  ScreenReturnType ret = SCREENx_RET_OK;
  
  switch (btn)
  {
    case BTN_DOWN:
      if (time_set == day)
      {
        if (--d < 0) d = 0;
      }
      else if (time_set == hour)
      {
        if (--h < 0) h = 59;
      }
      else if (--m < 0) 
        m = 59;
      xQueueSend( xQueueCursor,  &time_set, 0); // для активации задачи
      break;
    
    case BTN_UP:
      if (time_set == day)
      {
        if (++d > 24) d = 24;
      }
      else if (time_set == hour)
      {
        if (++h > 23) h = 0;
      }
      else if (++m > 59)
        m = 0;
      xQueueSend( xQueueCursor,  &time_set, 0); // для активации задачи
      break;
    
    case BTN_SET:
      if (time_set == day)
      {
        time_set = hour;
        xQueueSend( xQueueCursor,  &time_set, 0);
      }
      else if (time_set == hour)
      {
        time_set = min;
        xQueueSend( xQueueCursor,  &time_set, 0);
      }
      else
      {
        GV.measure_time_set = d*24*60 + h*60 + m;
        DeInitScreen();
        ret = SCREENx_RETURN;
      }
      break;

    case BTN_USER:
    case BTN_SET_LONG:
      GV.measure_time_set = d*24*60 + h*60 + m;
      DeInitScreen();
      ret = SCREENx_RETURN;
      break;
    
    default:
      break;
  }
  
  return ret;
}

// Инициализация экрана
void Screen9Init(gColor bc, gColor c)
{
  bgcolor = bc;
  color = c;
  
  d = GV.measure_time_set / (60*24);
  h = (GV.measure_time_set - d*24*60)/60;
  m = GV.measure_time_set - d*24*60 - h*60;
  
  time_set = day;
  
  gdispFillArea(screen_area.p.x, screen_area.p.y, 
                screen_area.size.x, screen_area.size.y, bgcolor);
  
  font_t font_txt = gdispOpenFont("times18bw");
  
  gdispDrawStringBox(0, 30, 138, 25,
    "Длительность", font_txt, color, gJustifyCenter);
  
  gdispDrawStringBox(0, 90, 138, 25,
    "Сут/Час/Мин", font_txt, color, gJustifyCenter); 
  
  gdispCloseFont(font_txt);
  
  DrawIcoUp(ico_up.x, ico_up.y, Black);
  DrawIcoDown(ico_down.x, ico_down.y, Black);
  DrawEnter(ico_enter.x, ico_enter.y, Black);
  
  font = font3;
    
  xQueueCursor = xQueueCreate( 1, sizeof( time_set ) );
  MY_CHECK( xQueueCursor ) ;
    
  xTaskCreate( taskNumberBlink, "NumberBlink", 256, NULL, 1, &xHandleNumberBlink);
  MY_CHECK( xHandleNumberBlink ) ;
  
  //enum type_time time_set = day;
}
