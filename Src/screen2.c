/*
Экран во время записи обследования
*/

#include <string.h>
#include "screen2.h"
#include "utils.h"
#include "DrawIco.h"
#include "hooks.h"
#include "measure.h"

extern GlobValType GV;

extern gFont font1; // малый шрифт - UI2 (h=8pxls)
extern gFont font2; // средний шрифт - times12bw (с кириллицей, h=12pxls)
extern gFont font3; // средний шрифт - times12bw (с кириллицей, h=12pxls)

static const Area screen_area = {{0,16},{160,112}}; // активная область экрана {start point, size}

static gColor bgcolor = White;        // цвет фона
static gColor color = Black;          //

// очередь сообщений для задачи обновления экрана                            
static QueueHandle_t xQueueTimeChage = NULL;
// handle для задачи обновления экрана 
static TaskHandle_t xHandleTimeChage = NULL;

static const gPoint leads_ref_point = {129,16};

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

static void TimeConvert(int32_t sec_total, TimeSpan* pts)
{
  pts->d = sec_total / (24*60*60);
  pts->h = (sec_total - (pts->d)*(24*60*60))/(60*60);
  pts->m = (sec_total - (pts->d)*(24*60*60) - (pts->h)*(60*60))/60;
  pts->s = (sec_total - (pts->d)*(24*60*60) - (pts->h)*(60*60) - (pts->m)*60);
}

/*
Задача вывода на экран мигающих цифр
*/                                  
static void taskTimeChage(void * argument)
{
  // Начальная точка вывода
  gCoord fh = gdispGetFontMetric(font3, gFontHeight);
  gCoord fw_number = gdispGetStringWidth("0", font3);
  gCoord fw_dot = gdispGetStringWidth(":", font3);
  gPoint ref_point, p;
  
  ref_point.x = (leads_ref_point.x - screen_area.p.x - fw_number*8 - fw_dot*2)/2 - 5;
  if (ref_point.x < 1)
    ref_point.x = 1;
  ref_point.y = screen_area.size.y / 2;
  p.x = ref_point.x;
  p.y = ref_point.y;
  
  int32_t time = 0;             // время измерения в сек
  TimeSpan ts;                  //
  TimeSpan ts_old;              // время измерения
  
  char ch_buff[12] = {0};
  char *str;
  int16_t leads_mask = 0xFFFF;      // маска для отведений, легенда которых будет перерисована
  int16_t leads_state_mask_old = 0; // вспом переменная для поиска leads_mask
  int16_t leads_state_mask_new;
  
  uint8_t string_redraw = 1;    // флаг необходимости перерисовки строки целиком
  
  ts_old.d = ts_old.h = ts_old.m = ts_old.s = -1;
  
  while(1)
  {
    TimeConvert(time, &ts);
    
    // дни
    if (ts.d != ts_old.d)
    {
      // при достижении 10дней, строку перерисовать
      // 2-е после or условие введено для корректной работе при отладке
      if ( ((ts_old.d < 10) & (ts.d >= 10)) | ((ts_old.d >= 10) & (ts.d < 10)) )
        string_redraw = 1;
      Int8ToStr(ts.d, &ch_buff[0]);
    }
    // часы
    if (ts.h != ts_old.h)
      Int8ToStr(ts.h, &ch_buff[3]);
    // минуты
    if (ts.m != ts_old.m)
      Int8ToStr(ts.m, &ch_buff[6]);
    // секунды
    if (ts.s != ts_old.s)
      Int8ToStr(ts.s, &ch_buff[9]);

    
    if (string_redraw)
    { // нужен вывод полной строки
      if (ts.d > 9)
        str = &ch_buff[0];
      else
        str = &ch_buff[1];
      ch_buff[2] = ch_buff[5] = ch_buff[8] = ':';
      gCoord sw = gdispGetStringWidth(str,font3); // длина строки
      // вывод строки
      gdispFillStringBox(ref_point.x, ref_point.y, sw+2, fh,
        str, font3, color, bgcolor, gJustifyLeft | gJustifyTop);
      ch_buff[2] = ch_buff[5] = ch_buff[8] = ch_buff[11] = 0; // символ конец строки
      string_redraw = 0;
    }
    else
    { // для уменьшения мерцания выведу только те позиции, которые изменились
      
      p.x = ref_point.x; p.y = ref_point.y;
      // сутки
      if (ts.d != ts_old.d)
      {
        if (ts.d > 9)
        {
          str = &ch_buff[0];
          gdispFillStringBox(p.x, p.y, fw_number*2 + 2, fh+1,
            str, font3, color, bgcolor, gJustifyLeft | gJustifyTop);
        }
        else
        {
          str = &ch_buff[1];
          gdispFillStringBox(p.x, p.y, fw_number + 2, fh+1,
            str, font3, color, bgcolor, gJustifyLeft | gJustifyTop);
        }
      }
      // корреккция коорд
      if (ts.d > 9)
        p.x += fw_number*2 + fw_dot;
      else
        p.x += fw_number + fw_dot;
      // часы
      if (ts.h != ts_old.h)
      {
        str = &ch_buff[3];
        gdispFillStringBox(p.x, p.y, fw_number*2 + 2, fh+1,
          str, font3, color, bgcolor, gJustifyLeft | gJustifyTop);
      }
      // корреккция коорд
      p.x += fw_number*2 + fw_dot;
      // минуты
      if (ts.m != ts_old.m)
      {
        str = &ch_buff[6];
        gdispFillStringBox(p.x, p.y, fw_number*2 + 2, fh+1,
          str, font3, color, bgcolor, gJustifyLeft | gJustifyTop);
      }
      // корреккция коорд
      p.x += fw_number*2 + fw_dot;
      // секунды
      if (ts.s != ts_old.s)
      {
        str = &ch_buff[9];
        gdispFillStringBox(p.x, p.y, fw_number*2 + 2, fh+1,
          str, font3, color, bgcolor, gJustifyLeft | gJustifyTop);
      }
    } // end else - вывод только изменившихся позиций

    ts_old = ts;
    
    // вывести легенду отведений
    DrawLeads(leads_ref_point, leads_mask, 0xFF, color, bgcolor);
    
    // поиск отведений, легенда которых требует перерисовки
    leads_state_mask_new = 0;
    for(int i = 0; i < ECG_LEADs_CNT; i++)
    {
      if(GV.ecg_lead[i].ecg_quality == ECG_ERR)
        leads_state_mask_new |= (1 << i);
    }
    leads_mask = leads_state_mask_new ^ leads_state_mask_old;
    leads_state_mask_old = leads_state_mask_new;

    // ожидание нового запуска
    xQueueReceive( xQueueTimeChage, &time, portMAX_DELAY);
  }
}

/*
Ф-ция, регистрируемая для обновления экрана измерений
*/
void CbMeasureScreenUpdate(uint32_t time_sec)
{
  if (xQueueTimeChage != NULL)
    xQueueSend( xQueueTimeChage,  &time_sec, 0);
}

static void DeInitScreen()
{
  vTaskDelete( xHandleTimeChage );
  xHandleTimeChage = NULL;
  vQueueDelete( xQueueTimeChage );
  xQueueTimeChage = NULL;
}
                        
// обработка событий экрана
ScreenReturnType Screen2Pool(ButtonPushType btn)
{
  ScreenReturnType ret = SCREENx_RET_OK;
  
  switch (btn)
  {    
    case BTN_SET:
      DeInitScreen();
      ret = SCREENx_RET_TO_SCREEN3;
      break;

    case BTN_USER:
    case BTN_SET_LONG:
      break;
    
    default:
      break;
  }
  
  return ret;
}

// Инициализация экрана
void Screen2Init(gColor bc, gColor c)
{
  bgcolor = bc;
  color = c;
  //uint16_t leads_mask;
  //uint8_t lead_iactive;
    
  gdispFillArea(screen_area.p.x, screen_area.p.y, 
                screen_area.size.x, screen_area.size.y, bgcolor);
  
  gdispDrawStringBox(0, 27, 138, 25,
    "Время записи", font2, color, gJustifyCenter);
  
  // кнопка
  gdispFillStringBox(23, 97, 94, 20,"Опции", font2, c, bc, gJustifyCenter | gJustifyTop);
  gdispDrawBox(23, 97, 94, 20, c);
  
  // вывести легенду отведений
  //leads_mask = 0xFFFF;  // перерисовка всей легенды 
  //lead_iactive = 0xFF;  // нет выделения какого то одного отведния
  //DrawLeads(leads_ref_point, leads_mask, lead_iactive, c, bc);
  
  xQueueTimeChage = xQueueCreate( 2, sizeof( uint32_t ) );
  MY_CHECK( xQueueTimeChage ) ;
  
  // Задача по обновлению времени
  xTaskCreate( taskTimeChage, "TimeChage", 256, NULL, 1, &xHandleTimeChage);
  MY_CHECK( xHandleTimeChage ) ;
  
  if (!MeasureIsRun())
  { // измерение не активно
    MeasureRegisterUpdateCb(CbMeasureScreenUpdate);
    MeasureStart(NULL);
  }
}
