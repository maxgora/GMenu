/*
Экран действий во время записи
*/

#include "screen3.h"
#include "DrawIco.h"

extern font_t font1; // малый шрифт - UI2 (h=8pxls)
extern font_t font2; // средний шрифт - times12bw (с кириллицей, h=12pxls)
extern font_t font3; // крупный шрифт - fixed_10x20 (для времени)

static const Area screen_area = {{0,16},{160,112}}; // активная область экрана {start point, size}

static gColor bg_color = White;       // цвет фона
static gColor color = Black;          // 

static const uint8_t menu_size = 4;     // максимальный размер меню
static const uint8_t menu_view_max = 4; // максимальное кол-во элементов меню, выводимое на экран
static uint8_t menu_itop = 0;           // индекс меню, отображаемого сверху
static uint8_t menu_iactive = 0;        // индекс активного меню

static const gPoint ico_up = {146,26};
static const gPoint ico_enter = {140,66};
static const gPoint ico_down = {146,106};

static const MenuType menu[menu_size] = { 
  { "Назад",      {{32,27},{94,17}},  NULL,NULL, SCREENx_RET_TO_SCREEN2 }, 
  { "ЭКГ",        {{0,0},{94,17}},    NULL,NULL, SCREENx_RET_TO_SCREEN5 },  
  { "Bluetooth",  {{0,0},{94,17}},BluetoothSwitch,NULL, SCREENx_RET_OK },  
  { "Остановить", {{0,0},{94,17}},    NULL,NULL, SCREENx_RET_TO_SCREEN4 } 
};

/*
Вывод меню на экран
Параметр:
mask - маска для элементов, требующих перерисовки
*/
static void MenuUpdate( uint16_t mask )
{
  gFont font = font2;
  Area a = {{32,27},{94,17}}; // область для вывода первого элемента меню
  uint8_t dy = 25;            // приращение y-координаты
  gColor icolor;
  uint16_t one_sh = 1;
  
  // проверка и корректировка списка меню
  if (menu_iactive > menu_size-1)
    menu_iactive = menu_size-1;
  if (menu_iactive < menu_itop)
    menu_itop = menu_iactive;
  else if ((menu_itop + menu_view_max - 1) < menu_iactive)
    menu_itop = menu_iactive - (menu_view_max-1);
  
  for(int i = menu_itop; i < (menu_itop + menu_view_max); i++)
  {
    if (one_sh & mask)
    { // для i-го элемента меню тр-ся перерисовка
      if (i == menu_iactive)
        icolor = Blue;
      else
        icolor = color;
        
      gdispFillStringBox(a.p.x, a.p.y, a.size.x, a.size.y, 
                          menu[i].name, font, icolor, bg_color, gJustifyCenter | gJustifyTop);  // текст
      
      gdispDrawBox(a.p.x, a.p.y, a.size.x, a.size.y, icolor);                               // рамка
    }   
    a.p.y += dy;
    // маска для след элемента меню
    one_sh = one_sh << 1;
  }
}
                        
                        
// обработка событий экрана
ScreenReturnType Screen3Pool(ButtonPushType btn)
{
  ScreenReturnType ret = SCREENx_RET_OK;
  uint8_t menu_iactive_old;
  uint32_t val;
  switch (btn)
  {
    case BTN_DOWN:
      if (menu_iactive < (menu_size-1))
      {
        menu_iactive_old = menu_iactive;
        menu_iactive++;
        MenuUpdate((0x0001<<menu_iactive)|(0x0001<<menu_iactive_old));
      }
      break;
    
    case BTN_UP:
      if (menu_iactive > 0)
      {
        menu_iactive_old = menu_iactive;
        menu_iactive--;
        MenuUpdate((0x0001<<menu_iactive)|(0x0001<<menu_iactive_old));
      }
      break;
    
    case BTN_SET:
      if (menu[menu_iactive].cbfunc != NULL)
      {
        val = menu[menu_iactive].cbfun_param;
        menu[menu_iactive].cbfunc(&val);
      }
      ret = menu[menu_iactive].ret;
      break;
    
    default:  // PUSH_USER
      break;
  }
  
  return ret;
}

// Инициализация экрана
void Screen3Init(gColor bc, gColor c)
{  
  bg_color = bc;
  color = c;
  
  gdispFillArea(screen_area.p.x, screen_area.p.y, 
                screen_area.size.x, screen_area.size.y, bg_color);
  
  DrawIcoUp(ico_up.x, ico_up.y, Black);
  DrawIcoDown(ico_down.x, ico_down.y, Black);
  DrawEnter(ico_enter.x, ico_enter.y, Black);
  // прорисовка всех элементов меню
  MenuUpdate(0xFFFF);
}

