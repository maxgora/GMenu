#include "screen10.h"
#include "DrawIco.h"

extern font_t font1; // малый шрифт - UI2 (h=8pxls)
extern font_t font2; // средний шрифт - times12bw (с кириллицей, h=12pxls)
extern font_t font3; // крупный шрифт - fixed_10x20 (для времени)

static const Area screen_area = {{0,16},{160,112}}; // активная область экрана {start point, size}

static gColor bg_color = White;       // цвет фона
static gColor color = Black;          // 

static const uint8_t menu_size = 7;     // максимальный размер меню
static const uint8_t menu_view_max = 4; // максимальное кол-во элементов меню, выводимое на экран
static uint8_t menu_itop = 0;           // индекс меню, отображаемого сверху
static uint8_t menu_iactive = 0;        // индекс активного меню

static const gPoint ico_up = {146,26};
static const gPoint ico_enter = {140,66};
static const gPoint ico_down = {146,106};

static char *pmenu[menu_size] = {"Старт",
                        "ЭКГ",
                        "Bluetooth",
                        "Выключить",
                        "Настройка обсл.",
                        "Уст. даты/время",
                        "Обновить ПО"}; 

static void MenuUpdate( void )
{
  Area a = {{32,27},{94,17}}; // площадь для вывода первого элемента меню
  uint8_t dy = 25;            // приращение y-координаты
  //gJustify j = gJustifyCenter;
  uint8_t j;                  // сдвиг области текста активного элемента меню
  
  // проверка и корректировка списка меню
  if (menu_iactive > menu_size-1)
    menu_iactive = menu_size-1;
  if (menu_iactive < menu_itop)
    menu_itop = menu_iactive;
  else if ((menu_itop + menu_view_max - 1) < menu_iactive)
    menu_itop = menu_iactive - (menu_view_max-1);
  
  for(int i = menu_itop; i < (menu_itop + menu_view_max); i++)
  {
    if (i == menu_iactive)
      j = 2;
    else
      j = 1;
    
    if ((i == 4) | (i == 5))
    { 
      gdispDrawBox(a.p.x-5, a.p.y, a.size.x+10, a.size.y, color);         // широкая рамка
      
      gdispFillStringBox(a.p.x+j-5, a.p.y+j, a.size.x-2+10, a.size.y-2, 
                          pmenu[i], font2, color, bg_color, gJustifyCenter);
    }
    else
    { 
      gdispDrawBox(a.p.x-5, a.p.y, a.size.x+10, a.size.y, bg_color);      // стереть широкую рамка
      
      gdispDrawBox(a.p.x, a.p.y, a.size.x, a.size.y, color);              // стандартная рамка
      
      gdispFillStringBox(a.p.x+j, a.p.y+j, a.size.x-2, a.size.y-2, 
                          pmenu[i], font2, color, bg_color, gJustifyCenter); // текст
    }
    
    a.p.y += dy;
  }
}
                        
                        
// обработка событий экрана
ScreenReturnType Screen10Pool(ButtonPushType btn)
{
  ScreenReturnType ret = SCREENx_RET_OK;
  switch (btn)
  {
    case BTN_DOWN:
      if (menu_iactive < (menu_size-1))
      {
        menu_iactive++;
        MenuUpdate();
      }
      break;
    
    case BTN_UP:
      if (menu_iactive > 0)
      {
        menu_iactive--;
        MenuUpdate();
      }
      break;
    
    case BTN_SET:
      switch (menu_iactive)
      {
        case 0: ret = SCREENx_RET_TO_SCREEN3; break;
        case 1: ret = SCREENx_RET_TO_SCREEN7; break;
        case 2: ret = SCREENx_RET_OK; break;
        case 3: ret = SCREENx_RET_OK; break;
        case 4: ret = SCREENx_RET_TO_SCREEN8; break;
        case 5: ret = SCREENx_RET_OK; break;
        case 6: ret = SCREENx_RET_OK; break;
      }
      break;
    
    default:  // PUSH_USER
      break;
  }
  
  return ret;
}

// Инициализация экрана
void Screen10Init(gColor bc, gColor c)
{  
  bg_color = bc;
  color = c;
  
  gdispFillArea(screen_area.p.x, screen_area.p.y, 
                screen_area.size.x, screen_area.size.y, bg_color);
  
  DrawIcoUp(ico_up.x, ico_up.y, Black);
  DrawIcoDown(ico_down.x, ico_down.y, Black);
  DrawEnter(ico_enter.x, ico_enter.y, Black);
  
  MenuUpdate(); 
}

