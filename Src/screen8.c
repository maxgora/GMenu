/*
Экран изменения режима
*/

#include "screen8.h"
#include "DrawIco.h"
#include "measure.h"

extern GlobValType GV;

static const Area screen_area = {{0,16},{160,112}}; // активная область экрана {start point, size}

static gColor bg_color = White;       // цвет фона
static gColor color = Black;          // 

static const uint8_t menu_size = 2;     // максимальный размер меню
static uint8_t menu_iactive = 0;        // индекс активного меню

static const gPoint ico_up = {146,26};
static const gPoint ico_enter = {140,66};
static const gPoint ico_down = {146,106};

static void SetMode( void* pval );

static const MenuType menu[menu_size] = { 
  { "Холтер",     {{25,57},{0,0}},    SetMode, MODE_HOLTER, SCREENx_RETURN }, 
  { "Событийный", {{25,83},{94,17}},  SetMode, MODE_EVENT,  SCREENx_RETURN }  
};

static void SetMode( void* pval )
{
  GV.ecg_mode = *((EcgModeType*)pval);
}

/*
Вывод меню на экран
Параметр:
mask - маска для элементов, требующих перерисовки
*/
static void MenuUpdate( uint16_t mask )
{
  gColor icolor;
  uint16_t one_sh = 1;
  
  for(int i = 0; i < menu_size; i++)
  {
    if (one_sh & mask)
    { // для i-го элемента меню тр-ся перерисовка
      if (i == menu_iactive)
        icolor = color;
      else
        icolor = bg_color;
        
      gdispFillCircle(menu[i].area.p.x - 10, menu[i].area.p.y + 8, 5, icolor);
    }   
    // маска для след элемента меню
    one_sh = one_sh << 1;
  }
}

static void DeInitScreen()
{
  ;  
}                        
                        
// обработка событий экрана
ScreenReturnType Screen8Pool(ButtonPushType btn)
{
  ScreenReturnType ret = SCREENx_RET_OK;
  uint8_t menu_iactive_old;
  uint32_t val;
  switch (btn)
  {
    case BTN_DOWN:
      menu_iactive_old = menu_iactive;
      if (menu_iactive < (menu_size-1))
        menu_iactive++;
      else
        menu_iactive = 0;
      MenuUpdate((0x0001<<menu_iactive)|(0x0001<<menu_iactive_old));
      break;
    
    case BTN_UP:
      menu_iactive_old = menu_iactive;
      if (menu_iactive > 0)
        menu_iactive--;
      else
        menu_iactive = menu_size-1;
      MenuUpdate((0x0001<<menu_iactive)|(0x0001<<menu_iactive_old));
      break;
    
    case BTN_SET:
      if (menu[menu_iactive].cbfunc != NULL)
      {
        val = menu[menu_iactive].cbfun_param;
        menu[menu_iactive].cbfunc(&val);
      }
      ret = menu[menu_iactive].ret;
      DeInitScreen();
      break;
    
    default:  // PUSH_USER
      break;
  }
  
  return ret;
}

// Инициализация экрана
void Screen8Init(gColor bc, gColor c)
{  
  bg_color = bc;
  color = c;
  
  gdispFillArea(screen_area.p.x, screen_area.p.y, 
                screen_area.size.x, screen_area.size.y, bg_color);
  
  DrawIcoUp(ico_up.x, ico_up.y, Black);
  DrawIcoDown(ico_down.x, ico_down.y, Black);
  DrawEnter(ico_enter.x, ico_enter.y, Black);
  
  font_t font = gdispOpenFont("times18bw");
  
  gdispDrawStringBox(0, 28, 138, 25,
    "Режим работы", font, color, gJustifyCenter);
  
  for (int i = 0; i < menu_size; i++)
    gdispDrawString(menu[i].area.p.x, menu[i].area.p.y, menu[i].name, font, c);
  
  gdispCloseFont(font);
  
  if (GV.ecg_mode == MODE_HOLTER)
    menu_iactive = 0;
  else
    menu_iactive = 1;
  
  // прорисовка всех элементов меню
  MenuUpdate(0xFFFF);
}

