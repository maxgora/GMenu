#include "header.h"
#include "stm32f2xx_hal_rtc.h"


extern RTC_HandleTypeDef hrtc;

extern GlobValType GV;

extern gFont font1; // малый шрифт - UI2 (h=8pxls)
extern gFont font2; // средний шрифт - times12bw (с кириллицей, h=12pxls)
extern gFont font3; // крупный шрифт - fixed_10x20 (для времени)

static gColor bg_color = White;       // цвет фона
static gColor txt_color = SkyBlue;    // цвет шрифта

#define FULL_ACCUM_LENGTH 20
//static int8_t accum_pecent = 100;
//static int8_t bt_on = 1;

static TaskHandle_t xHandleTaskHeader = NULL;

static char data_time[] = {'0','0',':','0','0', ' ','0','1','/','0','1','/','1','9'};

// задача обновления даты/времени
void taskHeaderUpdate(void* argument)
{
  gFont font = font2;
  RTC_TimeTypeDef time;
  RTC_DateTypeDef data;
  
  while (1)
  {
    ulTaskNotifyTake(pdTRUE,pdMS_TO_TICKS(60000));
    
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &data, RTC_FORMAT_BCD);
    
    data_time[0] = '0' + ((time.Hours >> 4) & 0x0F);
    data_time[1] = '0' + (time.Hours & 0x0F);

    data_time[3] = '0' + ((time.Minutes >> 4) & 0x0F);
    data_time[4] = '0' + (time.Minutes & 0x0F);
    
    data_time[6] = '0' + ((data.Date >> 4) & 0x0F);
    data_time[7] = '0' + (data.Date & 0x0F);

    data_time[9] = '0' + ((data.Month >> 4) & 0x0F);
    data_time[10] = '0' + (data.Month & 0x0F);
    
    data_time[12] = '0' + ((data.Year >> 4) & 0x0F);
    data_time[13] = '0' + (data.Year & 0x0F);
    
    gdispFillString(4, 0, data_time, font, txt_color, bg_color);
  }
}

// старт обновления даты/времени
void HeaderTimeUpdate(void)
{
  if (xHandleTaskHeader != NULL)
    xTaskNotifyGive(xHandleTaskHeader);
}

// старт обновления состояния аккумулятора
void HeaderAccumUpdate(int8_t percent)
{
  //accum_pecent = percent;
  /*gdispImage img;
  gdispImageError err;
  int length;
  err = gdispImageOpenFile(&img, "bat_empty.bmp");
  err = gdispImageDraw(&img, 128, 3, 23, 13, 0, 0);
  gdispImageClose(&img);  
  length = ((int)FULL_ACCUM_LENGTH*percent) / 100;  // заполнение значка батареи в пикселях
  if ( length > 1 )
    gdispFillArea( (151-length), 3, length, 13, Black);*/

  uint16_t start_x = 128;
  uint16_t start_y = 1;
  int length;               // заполнение значка батареи в пикселях
  
  if (percent > 100)
    percent = 100;
   
  gdispDrawBox(start_x, start_y+4, 2, 5, txt_color);                  // рисую малый прямоугольник
  gdispDrawBox(start_x+2, start_y, FULL_ACCUM_LENGTH, 13, txt_color); // большой прямоугольник
  
  // заполнение в соотв с percent
  length = ((int)FULL_ACCUM_LENGTH*percent) / 100;
  if ( length > 1 )
    gdispFillArea( (start_x+2+FULL_ACCUM_LENGTH-length), start_y, length, 13, txt_color);
    
  
}

// управление значком bluetooth
void HeaderBtUpdate(SignalStateType state)
{  
  //bt_on = on;
  if (state == SIG_ON)
  {
    gdispImage img;
//  gdispImageError err;
    
    gdispImageOpenFile(&img, "bluetooth.bmp");
    gdispImageDraw(&img, 100, 1, 15, 15, 0, 0);
    gdispImageClose(&img);
  }
  else
  {
    gdispFillArea(100, 1, 15, 15, bg_color);
  }
}

// Инициализация работы с верхней строкой
void HeaderInit(gColor background, gColor text_color)
{
  bg_color = background;
  txt_color = text_color;
  
  gdispFillArea(0, 0, 160, 15, bg_color);
  
  xTaskCreate( taskHeaderUpdate, "HeaderUpdate", 512, NULL, tskIDLE_PRIORITY, &xHandleTaskHeader);
  
  HeaderBtUpdate(GV.bluetooth_state);
  HeaderAccumUpdate(GV.accum_percent);
  HeaderTimeUpdate();
}

