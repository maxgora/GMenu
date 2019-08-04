/*
Экран просмотра ЭКГ
*/

#include <stdlib.h>
#include <string.h>
#include "screen5.h"
#include "DrawIco.h"
#include "hooks.h"

extern gFont font1; // малый шрифт - UI2 (h=8pxls)
extern gFont font2; // средний шрифт - times12bw (с кириллицей, h=12pxls)
extern gFont font3; // крупный шрифт - fixed_10x20 (для времени)

extern GlobValType GV;

static uint8_t screen_active = 0;         // флаг активности экрана

static const Area screen_area = {{0,16},{160,112}}; // область экрана {start point, size}

#define PLOT_P_X 32
#define PLOT_P_Y 34
#define PLOT_SIZE_X 96
#define PLOT_SIZE_Y 90
 // область экрана для вывода графика
static const Area plot_area = {{PLOT_P_X,PLOT_P_Y},{PLOT_SIZE_X, PLOT_SIZE_Y}};   
// буфер для вычисления и размещения точек графика
// (статическое размещения для ускорения работы)
static int16_t plot_poin[PLOT_SIZE_X];

static gCoord x_axis_offset = 81; // смещение оси х относительно начальной точки области вывода графика

static gColor bg_color = White;           // цвет фона
static gColor color = Black;              // 
     
static uint8_t lead_iactive = 0;          // индекс активного отведения

static uint16_t x_scale_start;            // начальный коэф. масшабирования, зависит от длинны исходного массива
static uint16_t x_scale_cur;              // активный коэф. масшабирования 
static const uint16_t x_scale_max = 1;    // макс коэф. масшабирования = 1 (1 точка на графике соотв 1 выборка исходного массива)

static uint16_t y_scale_start;            // начальный коэф. масшабирования, зависит от амплитуды сигнала
static uint16_t y_scale_cur;              //
static const uint16_t y_scale_max = 1;    //

static const gPoint ico_hscale = {116,26};
static const gPoint ico_vscale = {128,98};
static const gPoint ico_down = {144,67};

static const gPoint leads_ref_point = {4,16};

/*
Вывод легенды для 10ти отведений.
Параметр:
leads_mask - маска для отведений, требующих перерисовки
*/
/*static void  DrawLeads(uint16_t leads_mask)
{
  gFont font = font1;
  gPoint txt = {4,16};
  Area marker = {{21,18},{8,8}};
  gCoord dy = 11;
  uint16_t one_sh = 1;
  int i;

  for (i = 0; i < ECG_LEADs_CNT; i++)
  { // перебор отведений
    if (one_sh & leads_mask)
    { // для i-го отведения тр-ся перерисовка легенды
      if (i == lead_iactive)
        gdispFillString(txt.x, txt.y, GV.ecg_lead[i].lead_name, font, bg_color, color);
      else
        gdispFillString(txt.x, txt.y, GV.ecg_lead[i].lead_name, font, color, bg_color);
      
      if (GV.ecg_lead[i].ecg_quality == ECG_NORMAL)
        gdispFillArea(marker.p.x, marker.p.y, marker.size.x, marker.size.y, Green);
      else
        gdispFillArea(marker.p.x, marker.p.y, marker.size.x, marker.size.y, Red);
    }
    // инк коорд у
    txt.y += dy;
    marker.p.y += dy;
    // маска для след отведения
    one_sh = one_sh << 1;
  }
}*/
/*
Определение начального масштаба оси X.
Параметры:
pecg - входные данные
size_x - размер оси X
pscale_x - указатель на коэф масштабирования
*/
static void CalcAxisX(EcgArrType* pecg, gCoord size_x, uint16_t* pscale_x)
{
  uint16_t scale;
  
  // коэф по x
  scale = pecg->cnt / size_x;
  if ((scale * size_x) < pecg->cnt)
    scale++;
  *pscale_x = scale; 
}

/*
Определение начального масштаба оси Y и
смещения оси X относительно начальной точки области вывода графика.
Параметры:
data - входные данные
size.x - кол-во элементов входных данных
size.y - размер оси Y
pscale_y - указатель на коэф масштабирования
px_axis_offs - указатель на смещение оси Х
*/
static void CalcAxisY(int16_t* data, gPoint size, uint16_t* pscale_y, gCoord *px_axis_offs)
{
  int i;
  int16_t y_max = 0;
  int16_t y_min = 0;
  int16_t src_y_amp;
  uint16_t scale;
  
  for (i = 0; i < size.x; i++)
  {
    // max&min
    if (data[i] > y_max) y_max = data[i];
    if (data[i] < y_min) y_min = data[i];
  }
  // амплитуда сигнала
  src_y_amp = y_max - y_min;
  // коэф по у
  scale = src_y_amp / size.y;
  if ((scale * size.y) < src_y_amp)
    scale++;
  *pscale_y = scale;
  // соотв. значения max&min и оси у
  y_max = y_max / scale;
  y_min = y_min / scale;
  // смещение оси х
  *px_axis_offs = (size.y + y_min - 1);  
}

/*
Сжатие данных по Х
Параметры:
pbi - буфер входных данных;
start - начальный элемент для обработки входных данных;
pbo - буфер выходных данных;
pbo_size - макс кол-во элементов в pbo;
scale - коэф. масштабирования.
*/
void DataCompressX(EcgArrType* pbi, uint16_t start, int16_t *pbo, uint16_t pbo_size, uint16_t scale)
{
  int k = start;
  
  for (int i = 0; i < pbo_size; i++)
  {
    for (int s = 0; s < scale; s++)
    {
      pbo[i] += pbi->arr[k++];
      
      if (k >= pbi->cnt)
        return;
    }
  }
}

/*
Сжатие данных по Y
*/
void DataCompressY(int16_t *pbuff, uint16_t size, uint16_t scale)
{
  MY_CHECK( size <= (sizeof(plot_poin)/sizeof(plot_poin[0])) );
  
  for (int i = 0; i < size; i++)
  {
      pbuff[i] = pbuff[i]/scale; 
  }
}

// Рисование графика по данным из plot_poin
static void DrawPlotPoins()
{
  gPoint p0,p1;
  
  // очистка области графика
  gdispFillArea(plot_area.p.x, plot_area.p.y, plot_area.size.x, plot_area.size.y,  bg_color);
  // ось Х
  gdispDrawLine( plot_area.p.x,
              (plot_area.p.y + x_axis_offset),
              (plot_area.p.x + plot_area.size.x - 1),
              (plot_area.p.y + x_axis_offset),
                Blue);
  
  p0.x = plot_area.p.x;
  if (plot_poin[0] > (plot_area.p.y + x_axis_offset))
    p0.y = 0; // защита от смены разряда
  else
    p0.y = plot_area.p.y + x_axis_offset - plot_poin[0];
  
  for (int i = 1; i < plot_area.size.x; i++)
  {
    p1.x = plot_area.p.x + i;
    if (plot_poin[i] > (plot_area.p.y + x_axis_offset))
      p1.y = 0; // защита от смены разряда
    else
      p1.y = plot_area.p.y + x_axis_offset - plot_poin[i];
    
    // проверка допустимого диапазона и рисование точки графика
    //if ((p.y >= plot_area.p.y) || (p.y < plot_area.p.y + plot_area.size.y))
    //  gdispDrawPixel(p.x, p.y, color);
    
    /*if ( ((p0.y < plot_area.p.y) & (p1.y < plot_area.p.y)) | 
      ((p0.y > plot_area.p.y + plot_area.size.y - 1) & (p1.y > plot_area.p.y + plot_area.size.y - 1)) )
    { // обе точки вне границ и по одну сторону от оси Х
      ; // ничего не делаем - разрыв графика
    }
    else
    { // иначе, выходящую за границы точку привяжу к границе*/
      if (p0.y < plot_area.p.y)
        p0.y = plot_area.p.y;
      else if (p0.y > plot_area.p.y + plot_area.size.y - 1)
        p0.y = plot_area.p.y + plot_area.size.y - 1;
      
      if (p1.y < plot_area.p.y)
        p1.y = plot_area.p.y;
      else if (p1.y > plot_area.p.y + plot_area.size.y - 1)
        p1.y = plot_area.p.y + plot_area.size.y - 1;
      
      gdispDrawLine(p0.x, p0.y, p1.x, p1.y, color);
    //}

    p0.x = p1.x;
    p0.y = p1.y;
  }
}

/*
Обработка данных для вывода графика с масштабом по умолчанию
pecg - входные данные
Результат размещается в глобальных переменных:
x_scale_start, x_scale_cur, x_axis_offset
y_scale_cur, y_scale_cur  и глобальном буфере plot_poin[]
*/
static void GetDefaultPlotPoins(EcgArrType* pecg)
{
  uint16_t scale, size;
  gPoint buf_size;
  gCoord coord;
  
  MY_CHECK( pecg ) ;
  
  // очистка буфера для рассчета нового графика
  memset(plot_poin, 0, sizeof(plot_poin));
  
  // Определение начального масштаба оси X
  CalcAxisX(pecg, plot_area.size.x, &scale);
  // Ини стат переменных
  x_scale_start = scale;
  x_scale_cur = scale;
  // кол-во элементов массива выходных данных 
  size = pecg->cnt / scale;
  
  MY_CHECK(size <= plot_area.size.x);
  
  if (size > plot_area.size.x)
  { // если ошибка !!!
    size = plot_area.size.x;
  }
  
  // сжатие данных по Х
  DataCompressX(pecg, 0, plot_poin, size, scale);
  
  // Определение начального масштаба оси Y
  buf_size.x = size;
  buf_size.y = plot_area.size.y;  
  CalcAxisY ( plot_poin, buf_size, &scale, &coord);
  // Ини стат переменных
  y_scale_start = scale;
  y_scale_cur = scale;
  x_axis_offset = coord;
  // сжатие данных по Y
  DataCompressY(plot_poin, size, scale);
}

/*
Обработка данных для масштабирования графика.
Масштабирование по Y - относительно оси X,
по Х - относительно середины входных данных.
pecg - входные данные
Используются глобальные переменные:
x_scale_cur, x_axis_offset, y_scale_cur .
Результат размещается в глобальном буфере:
plot_poin[]
*/
static void GetScaledPlotPoins(EcgArrType* pecg)
{
  uint16_t size;
  uint16_t start;  // начальный элемент для обработки входных данных
  
  MY_CHECK( pecg ) ; 
  
  // очистка буфера для рассчета нового графика
  memset(plot_poin, 0, sizeof(plot_poin));
  
  // кол-во элементов массива выходных данных 
  size = pecg->cnt / x_scale_cur;    
  if (size > plot_area.size.x)
  { // если размер выходных данных при данном масштабировании превышает макс размер графика
    start = (size - plot_area.size.x)/2;  // коррекция начального элемента входных данных
    size = plot_area.size.x;              // коррекция количества обрабатываемых данных
  }
  else
    start = 0;
    
  // сжатие данных по Х
  DataCompressX(pecg, start, plot_poin, size, x_scale_cur);
  
  // сжатие данных по Y
  DataCompressY(plot_poin, size, y_scale_cur);
}

/*
Ф-ция, регистрируемая на вызов после обновления состояния 
!!! Лучше сделать задачу, запускаемую по событию, чтобы не тормозить МК
*/
static void CbLeadsUpdate (uint16_t leads_mask)
{
  if (!screen_active) return;
  
  // прорисовка легенды для всех отведений
  //DrawLeads(leads_mask);
  DrawLeads(leads_ref_point, leads_mask, lead_iactive, color, bg_color);
  // обработка данных с заданным масштабом
  GetScaledPlotPoins(&GV.ecg_lead[lead_iactive].ecg);
  // рисование графика
  DrawPlotPoins();
}
                        
/*
Обработка событий экрана.
Используются глобальные переменные:
lead_iactive,
x_scale_cur, x_scale_start, x_axis_offset,
y_scale_cur, y_scale_start
*/
ScreenReturnType Screen5Pool(ButtonPushType btn)
{
  ScreenReturnType ret = SCREENx_RET_OK;
  uint8_t lead_iactive_old;
  uint16_t leads_mask;
  
  switch (btn)
  {
     // смена  отведения
    case BTN_SET:
      lead_iactive_old = lead_iactive;
      if (lead_iactive < (ECG_LEADs_CNT-1))
        lead_iactive++;
      else
        lead_iactive = 0;
      // перерисовка легенды отведений 
      //DrawLeads((0x0001<<lead_iactive)|(0x0001<<lead_iactive_old));
      leads_mask = (0x0001<<lead_iactive)|(0x0001<<lead_iactive_old);
      DrawLeads(leads_ref_point, leads_mask, lead_iactive, color, bg_color);
      // Обработка данных для вывода нового графика с масштабом по умолчанию
      GetDefaultPlotPoins(&GV.ecg_lead[lead_iactive].ecg);
      // рисование графика
      DrawPlotPoins();
      break;

    // изменение масштаба оси Х по кругу
    case BTN_UP:  
      x_scale_cur--;
      // проверка допустимого значения x_scale_cur
      if ((x_scale_cur < x_scale_max) | (x_scale_cur > x_scale_start))
        x_scale_cur = x_scale_start;
      // обработка данных с заданным масштабом
      GetScaledPlotPoins(&GV.ecg_lead[lead_iactive].ecg);
      // рисование графика
      DrawPlotPoins();      
      break;
    
    // изменение масштаба оси Y по кругу
    case BTN_DOWN:  
      y_scale_cur--;
      // проверка допустимого значения x_scale_cur
      if ((y_scale_cur < y_scale_max) | (y_scale_cur > y_scale_start))
        y_scale_cur = y_scale_start;
      // обработка данных с заданным масштабом
      GetScaledPlotPoins(&GV.ecg_lead[lead_iactive].ecg);
      // рисование графика
      DrawPlotPoins();
      break;
      
      // выход из текущего меню
    case BTN_USER:  //
    case BTN_SET_LONG:
      screen_active = 0;
      // GV.CbEcgUpdateHandler = NULL; 
      ret = SCREENx_RETURN;
      break;
    
    default:  // PUSH_USER
      break;
  }
  
  return ret;
}

// Инициализация экрана
void Screen5Init(gColor bc, gColor c)
{
  uint16_t leads_mask;
  
  screen_active = 1;
  GV.CbEcgUpdateHandler = CbLeadsUpdate;
  
  bg_color = bc;
  color = c;
  // очистка экрана
  gdispFillArea(screen_area.p.x, screen_area.p.y, 
                screen_area.size.x, screen_area.size.y, bg_color);
  // иконки действий
  DrawHScaleFill(ico_hscale.x, ico_hscale.y, Black);
  DrawVScaleFill(ico_vscale.x, ico_vscale.y, Black);
  DrawIcoDown(ico_down.x, ico_down.y, Black);
  // прорисовка легенды для всех отведений
  leads_mask = 0xFFFF;
  DrawLeads(leads_ref_point, leads_mask, lead_iactive, color, bg_color);
  // Обработка данных для вывода графика
  GetDefaultPlotPoins(&GV.ecg_lead[lead_iactive].ecg);
  // рисование графика
  DrawPlotPoins();
}

