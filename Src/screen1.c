/*
Экран просмотра настроек обследования перед записью
*/

#include <string.h>
#include "screen1.h"
#include "DrawIco.h"
#include "utils.h"

extern GlobValType GV;

extern gFont font1; // малый шрифт - UI2 (h=8pxls)
extern gFont font2; // средний шрифт - times12bw (с кириллицей, h=12pxls)
extern gFont font3; // крупный шрифт - fixed_10x20 (для времени)

static const Area screen_area = {{0,16},{160,112}}; // активная область экрана {start point, size}

static gColor bg_color = White;       // цвет фона
static gColor color = Black;          // 

static uint8_t menu_set = 0;            // вариант меню
static const uint8_t menu_set_cnt = 3;
static const uint8_t menu2_cnt = 3;     // максимальный размер меню2
static uint8_t menu_iactive = 1;        // индекс активного меню

static const gPoint ico_up = {146,26};
static const gPoint ico_enter = {140,66};
static const gPoint ico_down = {146,106};
static const Area marker = {{7,0},{4,0}};

// Определение областей вывода меню
// Формат : {"Описание", {{poin_x,poin_y},{size_x,size_y}}}
// значение координаты, равное 0 означает, что значение вычисляется

static const MenuType menu0[] = { 
  { "ФИО: ",            {{0,33},{0,0}},     NULL,NULL, SCREENx_RET_OK },  
  { "Режим: ",          {{0,0},{0,0}},      NULL,NULL, SCREENx_RET_OK }
};
static const MenuType menu1[] = { 
  { "Длительность: ",   {{0,27},{0,0}},     NULL,NULL, SCREENx_RET_OK },  
  { "Частота дискр.: ", {{0,0},{0,0}},      NULL,NULL, SCREENx_RET_OK }, 
  { "Пейсмейкер: ",     {{0,0},{0,0}},      NULL,NULL, SCREENx_RET_OK }
};  
static const MenuType menu2[] = { 
  { "Назад",            {{27,30},{104,0}},  NULL,NULL, SCREENx_RET_TO_SCREEN0 }, 
  { "Изменить",         {{27,60},{104,0}},  NULL,NULL, SCREENx_RET_TO_SCREEN6 },  
  { "Начать",           {{27,90},{104,0}},  NULL,NULL, SCREENx_RET_TO_SCREEN2 } 
};

static void Menu0Update( void )
{
  gFont font = font2;
  Area a;               // для вычисления области вывода
  char dst[64];         // буфер для формирования выходной строки
  char* str;
  gCoord sw;            // string width
  gCoord sh = gdispGetFontMetric(font, gFontHeight); // string hegth
  
  // * ФИО ... (способ 1 - две области - пункт меню и наполнение)
  // 
  a.p.x = menu0[0].area.p.x ? menu0[0].area.p.x : (marker.p.x + marker.size.x + 4);
  a.p.y = menu0[0].area.p.y ? menu0[0].area.p.y : (screen_area.p.y + 2);
  gdispDrawString(a.p.x, a.p.y, menu0[0].name, font, color);
  sw = gdispGetStringWidth(menu0[0].name, font);
  // вычисление допустимой области для вывода имени
  a.p.x += sw;
  // a.p.y - без изменения
  a.size.x = ico_enter.x - a.p.x - 1;
  a.size.y = menu0[0].area.size.y ? menu0[0].area.size.y : (sh*3 + 3); // size.y = заданное значение или 3и строки
  // вывод имени
  gdispFillStringBox(a.p.x, a.p.y,
                    a.size.x,a.size.y ,
                    GV.full_name,
                    font,
                    Black,
                    bg_color,//SkyBlue,//
                    gJustifyTop);
  gdispFillCircle(marker.p.x, a.p.y+sh/2, marker.size.x/2, color);  // маркер
  
  // * Режим ... (способ 2 - одной строкой с переносом)
  //
  str = strcpy(dst, menu0[1].name);           // "Режим: "
  if (GV.ecg_mode == MODE_HOLTER)
    str = strcat(dst, "Холтер");              // "Режим: Холтер"
  else
    str = strcat(dst, "Событийный");          // "Режим: Событийный"
  // область вывода
  a.p.x = menu0[1].area.p.x ? menu0[1].area.p.x : (marker.p.x + marker.size.x + 4);
  a.p.y = menu0[1].area.p.y ? menu0[1].area.p.y : (a.p.y + a.size.y);
  a.size.x = menu0[1].area.size.x ? menu0[1].area.size.x : (ico_enter.x - a.p.x - 1);
  if (menu0[1].area.size.y == 0)
  { // авто вычисление размера по y
    a.size.y = sh + 3;  
    // проверка области вывода
    sw = gdispGetStringWidth(str, font);
    // корректировка области вывода (не более +1строка)
    if(sw > a.size.x)
      a.size.y += sh;
  }
  else
    a.size.y = menu0[1].area.size.y;
  // вывод строки
  gdispFillStringBox(a.p.x, a.p.y, a.size.x,a.size.y ,
                    str, font, Black, bg_color,//SkyBlue,//
                    gJustifyTop);
  gdispFillCircle(marker.p.x, a.p.y+sh/2, marker.size.x/2, color);      // маркер
}

static void Menu1Update( void )
{
  gFont font = font2;
  Area a;               // для вычисления области вывода
  char number[11];      // буфер для перевода числа в строку
  char dst[64] = {0};   // буфер для формирования выходной строки
  char* str;
  uint16_t d,h,m;       // временные переменные
  gCoord sw;            // для подсчета длины строки
  gCoord sh = gdispGetFontMetric(font, gFontHeight); // string hegth
  
  // * Длительность ... (способ 1 - две области - пункт меню и наполнение)
  a.p.x = menu1[0].area.p.x ? menu1[0].area.p.x : (marker.p.x + marker.size.x + 4);
  a.p.y = menu1[0].area.p.y ? menu1[0].area.p.y : (screen_area.p.y + 2);
  gdispDrawString(a.p.x, a.p.y, menu1[0].name, font, color);
  sw = gdispGetStringWidth(menu1[0].name, font);
  // часы и минуты измерения
  d = GV.measure_time_set / (60*24);
  h = (GV.measure_time_set - d * 60*24)/ 60;
  m = GV.measure_time_set - d * 60*24 - h*60;
  // формирования выходной строки
  if (d > 0)
  {
    str = utoa_fast_div((uint32_t)d, number); 
    //str = strcpy(dst, str);                   // "XX"
    str = strcat(dst, str);                     // "XX"
    str = strcat(dst, "сут. ");                 // "XXсут. "
  }
  str = utoa_fast_div((uint32_t)h, number); 
  str = strcat(dst, str);                     // "XXсут. XX"
  str = strcat(dst, "ч. ");                   // "XXсут. XXч. "
  str = utoa_fast_div((uint32_t)m, number);
  str = strcat(dst, str);                     // "XXсут. XXч. ХХ"
  str = strcat(dst, "м.");                    // "XXсут. XXч. ХХм."
  // вычисление допустимой области для вывода "XXXч. ХХм."
  a.p.x += sw;  // +length("Длительность: ")
  // a.p.y - без изменения
  a.size.x = ico_enter.x - a.p.x - 1;
  if (menu1[0].area.size.y == 0)
  { // автовычисление размера по y
    a.size.y = sh + 3;
    // проверка области вывода
    sw = gdispGetStringWidth(str, font);
    // корректировка области вывода (не более +1строка)
    if(sw > a.size.x)
      a.size.y += sh;
  }
    else
      a.size.y = menu1[0].area.size.y;
  // вывод строки
  gdispFillStringBox(a.p.x, a.p.y, a.size.x,a.size.y ,
                    str, font, Black, bg_color,//SkyBlue,//
                    gJustifyTop);
  gdispFillCircle(marker.p.x, a.p.y+sh/2, marker.size.x/2, color);        // маркер
  
  // * Частота дискретизации ...
  // формирования выходной строки
  str = strcpy(dst, menu1[1].name);           // "Частота дискретизации: "
  str = utoa_fast_div((uint32_t)GV.smpl_rate, number); 
  str = strcat(dst, str);                     // "Частота дискретизации: ХXXX"
  str = strcat(dst, "Гц");                    // "Частота дискретизации: ХXXXГц"
  // область вывода
  a.p.x = menu1[1].area.p.x ? menu1[1].area.p.x : (marker.p.x + marker.size.x + 4);
  a.p.y = menu1[1].area.p.y ? menu1[1].area.p.y : (a.p.y + a.size.y);
  a.size.x = menu1[1].area.size.x ? menu1[1].area.size.x : (ico_enter.x - a.p.x - 1);
  if (menu1[1].area.size.y == 0)
  {
    a.size.y = sh + 3;//menu1[1].area.size.y;  
    // проверка области вывода
    sw = gdispGetStringWidth(str, font);
    // корректировка области вывода (не более +1строка)
    if(sw > a.size.x)
      a.size.y += sh;
  }
  else
    a.size.y = menu1[1].area.size.y;
  // вывод строки
  gdispFillStringBox(a.p.x, a.p.y, a.size.x,a.size.y ,
                    str, font, Black, bg_color,//SkyBlue,//
                    gJustifyTop);
  gdispFillCircle(marker.p.x, a.p.y+sh/2, marker.size.x/2, color);      // маркер

  // * Пейсмейкер ... 
  str = strcpy(dst, menu1[2].name);           // "Пейсмейкер: "
  if (GV.pacemaker_state == SIG_ON)
    str = strcat(dst, "Вкл.");                // "Пейсмейкер: Вкл."
  else
    str = strcat(dst, "Выкл.");               // "Пейсмейкер: Выкл."
  // область вывода
  a.p.x = menu1[2].area.p.x ? menu1[2].area.p.x : (marker.p.x + marker.size.x + 4);
  a.p.y = menu1[2].area.p.y ? menu1[2].area.p.y : (a.p.y + a.size.y);
  a.size.x = menu1[2].area.size.x ? menu1[2].area.size.x : (ico_enter.x - a.p.x - 1);
  if (menu1[2].area.size.y == 0)
  {
    a.size.y = sh + 3;  
    // проверка области вывода
    sw = gdispGetStringWidth(str, font);
    // корректировка области вывода (не более +1строка)
    if(sw > a.size.x)
      a.size.y += sh;
  }
  else
    a.size.y = menu1[2].area.size.y;
  // вывод строки
  gdispFillStringBox(a.p.x, a.p.y, a.size.x,a.size.y ,
                    str, font, Black, bg_color,//SkyBlue,//
                    gJustifyTop);
  gdispFillCircle(marker.p.x, a.p.y+sh/2, marker.size.x/2, color);        // маркер
}

static void Menu2Update( void )
{
  gFont font = font2;
  Area a = {0};             // задание области вывода
  gColor icolor;
  gCoord sw;                // для подсчета длины строки
  gCoord sh = gdispGetFontMetric(font, gFontHeight); // string hegth
  Area a_calc = {0};        // для вычисления области вывода
  gCoord dy = 7;            // расстояние м/д кнопками

  // вычисление габаритов кнопки
  for (int i = 0; i < menu2_cnt; i++)
  {
    sw = gdispGetStringWidth(menu2[i].name, font);
    if ( ( sw + 10) > a_calc.size.x )
      a_calc.size.x = sw + 10;
  }
  a_calc.size.y = sh + 2;
  a_calc.p.x = (ico_enter.x - screen_area.p.x - a_calc.size.x) / 2;
  a_calc.p.y = (screen_area.p.y + 5);
  // расстояние м/д кнопками
  dy = (screen_area.size.y - 10 - (a_calc.size.y * menu2_cnt)) / (menu2_cnt - 1); // -10 - кол-во пикселей снизу
  if (dy < 0)
    dy = 0;
  
  // область вывода
  a.p.x = menu2[0].area.p.x ? menu2[0].area.p.x : a_calc.p.x;
  a.p.y = menu2[0].area.p.y ? menu2[0].area.p.y : a_calc.p.y;
  a.size.x = menu2[0].area.size.x ? menu2[0].area.size.x : a_calc.size.x;
  a.size.y = menu2[0].area.size.y ? menu2[0].area.size.y : a_calc.size.y;
  
  int i = 0;
  while(i < menu2_cnt)
  {
    if (i == menu_iactive) icolor = Blue;
    else icolor = color;
    
    // текст
    gdispFillStringBox(a.p.x, a.p.y, a.size.x, a.size.y, 
                      menu2[i].name, font, icolor, bg_color, gJustifyCenter | gJustifyTop);
    // рамка
    gdispDrawBox(a.p.x, a.p.y, a.size.x, a.size.y, icolor);

    if (++i >= menu2_cnt)
      break;
    
    // область вывода след кнопки
    a.p.x = menu2[i].area.p.x ? menu2[i].area.p.x : a_calc.p.x;
    a.p.y = menu2[i].area.p.y ? menu2[i].area.p.y : (a.p.y + a.size.y + dy);
    a.size.x = menu2[i].area.size.x ? menu2[i].area.size.x : (a_calc.size.x);
    a.size.y = menu2[i].area.size.y ? menu2[i].area.size.y : (a_calc.size.y);                
  }
}

static void MenuUpdate( void )
{  
  if (menu_set > 2)
    { // указатели меню в ИС
    menu_set = 0;
    menu_iactive = 1;
  }
    
  gdispFillArea(0, 17, 139, 127, bg_color); // зачистка от старого меню
  
  if (menu_set == 0)
    Menu0Update();
  else if (menu_set == 1)
    Menu1Update();
  else
    Menu2Update();
}
                        
// обработка событий экрана
ScreenReturnType Screen1Pool(ButtonPushType btn)
{
  ScreenReturnType ret = SCREENx_RET_OK;
  switch (btn)
  {
    case BTN_DOWN:
      if (menu_set < (menu_set_cnt-1))
      {
        menu_set++;
        MenuUpdate();
      }
      else if (menu_iactive < (menu2_cnt-1))
      {
        menu_iactive++;
        MenuUpdate();
      }
      break;
    
    case BTN_UP:
      if ((menu_set == 2) & (menu_iactive > 0))
      {
        menu_iactive--;
        MenuUpdate();
      }
      else if (menu_set > 0)
      {
        menu_set--;
        MenuUpdate();
      }
      break;
    
    case BTN_SET:
      if (menu_set == 2)
        ret = menu2[menu_iactive].ret;
      break;
    
    default:  // PUSH_USER
      break;
  }
  
  return ret;
}

// Инициализация экрана
void Screen1Init(gColor bc, gColor c)
{
  bg_color = bc;
  color = c;
  
  gdispFillArea(screen_area.p.x, screen_area.p.y, 
                screen_area.size.x, screen_area.size.y, bg_color);
  
  DrawIcoUp(ico_up.x, ico_up.y, Black);
  DrawIcoDown(ico_down.x, ico_down.y, Black);
  DrawEnter(ico_enter.x, ico_enter.y, Black);
  
  menu_set = 0;
    
  MenuUpdate(); 
}

