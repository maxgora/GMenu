#include "screen6.h"
#include "DrawIco.h"
#include "header.h"

extern gFont font1; // малый шрифт - UI2 (h=8pxls)
extern gFont font2; // средний шрифт - times12bw (с кириллицей, h=12pxls)
extern gFont font3; // крупный шрифт - fixed_10x20 (для времени)

extern GlobValType GV;

static const Area screen_area = {{0,16},{160,112}}; // активная область экрана {start point, size}

static gColor bg_color = White;       // цвет фона
static gColor color = Black;          // 

static const uint8_t menu_size = 6;     // максимальный размер меню
static const uint8_t menu_view_max = 4; // максимальное кол-во элементов меню, выводимое на экран
static uint8_t menu_itop = 0;           // индекс меню, отображаемого сверху
static uint8_t menu_iactive = 0;        // индекс активного меню

static const gPoint ico_up = {146,26};
static const gPoint ico_enter = {140,66};
static const gPoint ico_down = {146,106};

static char *pmenu[menu_size] = {"Назад",
                        "ФИО",
                        "Режим",
                        "Длительность",
                        "Частота дискр",
                        "Пейсмейкер" }; 

static void MenuUpdate(ButtonPushType btn)
{
  gFont font = font2;
  gCoord sh = gdispGetFontMetric(font, gFontHeight); // string hegth
  Area a = {{27,26},{104,sh+3}};  // область для вывода первого элемента меню
  uint8_t dy = sh + 9;            // приращение y-координаты

  //gCoord string_width;
  //int16_t sval;
  gColor icolor;
  
  int cur_i = menu_itop;                      // первый элемент для обновления
  int stop_i = menu_itop + menu_view_max - 1; // последний элемент для обновления
  
  if (menu_iactive > menu_size-1)
    menu_iactive = menu_size-1;
  
  // проверка и корректировка списка меню
  if (menu_iactive < menu_itop)
  {
    menu_itop = menu_iactive;
    cur_i = menu_itop;
    stop_i = menu_itop + menu_view_max - 1;
  }
  else if ((menu_itop + menu_view_max - 1) < menu_iactive)
  {
    menu_itop = menu_iactive - (menu_view_max-1);
    cur_i = menu_itop;
    stop_i = menu_itop + menu_view_max - 1;
  }
  // корректировки списка нет - можно ограничить перерисовку 2мя элементами
  else if (btn == BTN_DOWN)
  { 
    cur_i = menu_iactive-1;     // первый элемент для обновления
    stop_i = menu_iactive;      // последний элемент для обновления
    a.p.y += (cur_i - menu_itop)*dy;  // смещение элемента по y
  }
  else if (btn == BTN_UP)
  {
    cur_i = menu_iactive;       // первый элемент для обновления
    stop_i = menu_iactive+1;    // последний элемент для обновления
    a.p.y += (cur_i - menu_itop)*dy;  // смещение элемента по y
  }
 
  while (cur_i <= stop_i)
  {       
    if (cur_i == menu_iactive)
      icolor = Blue;
    else
      icolor = color;
      
    gdispFillStringBox(a.p.x, a.p.y, a.size.x, a.size.y, 
                        pmenu[cur_i], font, icolor, bg_color, gJustifyCenter | gJustifyTop); // текст
    
    gdispDrawBox(a.p.x, a.p.y, a.size.x, a.size.y, icolor);                     // рамка
    
    a.p.y += dy;
    cur_i++;
  }
}
                        
                        
// обработка событий экрана
ScreenReturnType Screen6Pool(ButtonPushType btn)
{
  ScreenReturnType ret = SCREENx_RET_OK;
  switch (btn)
  {
    case BTN_DOWN:
      if (menu_iactive < (menu_size-1))
      {
        menu_iactive++;
        MenuUpdate(btn);
      }
      break;
    
    case BTN_UP:
      if (menu_iactive > 0)
      {
        menu_iactive--;
        MenuUpdate(btn);
      }
      break;
    
    case BTN_SET:
      switch (menu_iactive)
      {
        case 0: ret = SCREENx_RETURN; break;
        case 1: ret = SCREENx_RET_TO_SCREEN7; break;
        case 2: ret = SCREENx_RET_TO_SCREEN8; break;
        case 3: ret = SCREENx_RET_TO_SCREEN9; break;
        case 4: ret = SCREENx_RET_TO_SCREEN10; break;
        case 5: ret = SCREENx_RET_TO_SCREEN11; break;
      }
      break;
    
    default:  // PUSH_USER
      break;
  }
  
  return ret;
}

// Инициализация экрана
void Screen6Init(gColor bc, gColor c)
{  
  bg_color = bc;
  color = c;
  
  gdispFillArea(screen_area.p.x, screen_area.p.y, 
                screen_area.size.x, screen_area.size.y, bg_color);
  
  DrawIcoUp(ico_up.x, ico_up.y, Black);
  DrawIcoDown(ico_down.x, ico_down.y, Black);
  DrawEnter(ico_enter.x, ico_enter.y, Black);
  
  MenuUpdate(BTN_UNDEF); 
}

