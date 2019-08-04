/*
Экран изменения ФИО
Нажатие кнопок вверх/вниз - смена буквы.
Длинное нажатие кнопок - начать сначала.
Средняя кнопка - зафиксировать выбранный символ.
Длинное нажатие средней кнопки - ввод завершен и выход.

Прим.
- убрал cтрочные символы , т.к. ввод имени с ними не удобен;
*/

#include "screen7.h"
#include <string.h>
#include "DrawIco.h"
#include "hooks.h"

extern GlobValType GV;

static font_t font; // крупный шрифт - fixed_10x20 (для времени)

static const Area screen_area = {{0,16},{160,112}}; // активная область экрана {start point, size}
static gPoint char_place = {5,20};                  // место вывода символа

static gColor bg_color = White;       // цвет фона
static gColor color = Black;          // 

/*
static const char *ch_lower[] = {"а","б","в","г","д","е","ё","ж","з","и","й","к",
 "л","м","н","о","п","р","с","т","у","ф",
 "х","ц","ч","ш","щ","ы","э","ю","я","-"," "};
!!! Ошибка - при формировании массива компилятор пропускает "Г"(после "В" сразу следует "Д", причину не понял)
static const char *ch_upper[] = {"А","Б","В","Г","Д","Е","Ё","Ж","З","И","Й","К",
 "Л","М","Н","О","П","Р","С","Т","У","Ф",
 "Х","Ц","Ч","Ш","Щ","Ы","Э","Ю","Я","-"," "};*/

#define UPPER_CASE_SET "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЭЮЯ- "
static const char *ch_upper = {UPPER_CASE_SET};

static const char *pchar;     // указатель на строку с символом
 
static wchar_t full_name[FULL_NAME_MAX_CHAR];       // строка с новым именем

// очередь сообщений в обработчик курсора                               
QueueHandle_t xQueueCursor;
//                                  
static TaskHandle_t xHandleTaskCursorBlink = NULL;

/*
Задача вывода на экран мигающего курсора
*/                                  
static void taskCursorBlink(void * argument)
{
  gPoint cursor_cur = *((gPoint*)argument);
  gPoint cursor_new;
  gColor cursor_color = color;
  BaseType_t waite_res;
  gCoord h = gdispGetFontMetric(font, gFontHeight);
  
  while(1)
  {
    waite_res = xQueueReceive( xQueueCursor, &cursor_new, 500/portTICK_PERIOD_MS);
    
    if (waite_res == pdTRUE)
      cursor_color = bg_color;
    
    gdispDrawLine(cursor_cur.x, cursor_cur.y, cursor_cur.x, (cursor_cur.y + h), cursor_color);
    
    if (cursor_color == bg_color)
      cursor_color = color;
    else
      cursor_color = bg_color;
    
    if (waite_res == pdTRUE)
      cursor_cur = cursor_new;
  }
}


/*
Возвращает строку с одним элементом из набора смешанных символов
*/
static char *GetStr(const char *in, wchar_t *out)
{      
  out[1] = 0;
  if (*in & 0x80)
  { // двух байтовый символ
    out[0] = *((wchar_t*)in);
  }
  else
  { // одно байтовый символ
    ((char*)out)[0] = *in;
    ((char*)out)[1] = 0;
  }
  return (char*)out;
}
/*
Перемещение курсора в следующую позицию
Параметры
p - введённый символ
curs - текущая позиция курсора
*/
static void CursorToNextPosition(const char *pch, gPoint *curs)
{
  wchar_t wch_buff[2];
  char *str = GetStr(pch, wch_buff);
  
  gCoord sw = gdispGetStringWidth(str, font);
  
  if ((curs->x + sw + FONT_CH_MAXW) > (screen_area.size.x - screen_area.p.x))
  {
    curs->x = 5;         // след символ может выйти за границы экрана
    curs->y += FONT_CH_H;
  }
  else
    curs->x += sw;
  
  if ((curs->y + FONT_CH_H) > screen_area.size.y)
    curs->y = 20;       // символ может выйти за границы экрана
}

/*
Перемещение курсора на след строку
Параметры
p - введённый символ
curs - текущая позиция курсора
*/
static void CursorToNextLine(gPoint *curs)
{
  curs->x = 5;         // след символ может выйти за границы экрана
  curs->y += FONT_CH_H;
  
  if ((curs->y + FONT_CH_H) > screen_area.size.y)
    curs->y = 20;       // символ может выйти за границы экрана
}

/*
Переключение на строчные
*/
/*static void SwithCaseToUpper( const char *pch )
{
  uint16_t size = sizeof(ch_lower); // 124
  
  if ( (pch >= ch_lower[0]) & ( pch < (ch_lower[0] + size)) )
    pch = ch_upper[0] + (pch - ch_lower[0]);  // смена регистра
  else
    pch = ch_upper[0];
}*/
/*
Переключение на прописные символы
*/
/*static void SwithCaseToLower( const char *pch )
{
  uint16_t size = sizeof(ch_upper); // 124
  
  if ( (pch >= ch_upper[0]) & ( pch < (ch_upper[0] + size)) )
    pch = ch_lower[0] + (pch - ch_upper[0]);  // смена регистра
  else
    pch = ch_lower[0];
}*/
/*
Рисование на экране текущего символа
*/
static void DrawChar( const char *pch )
{
  gdispFillArea(char_place.x, char_place.y, FONT_CH_MAXW, (FONT_CH_H+FONT_CH_D), bg_color);
  
  wchar_t wch_buff[2];
  char *str = GetStr(pch, wch_buff);
  
  if  (*str == ' ')
    *str = '_';       // подмена пробела на подчеркивание
  gdispDrawString(char_place.x, char_place.y, str, font, color);
}
/*
Стереть символ 
*/
static void ClearChar( void )
{  
  //gCoord sw = gdispGetStringWidth(str, font); 
  gdispFillArea(char_place.x, char_place.y, FONT_CH_MAXW, (FONT_CH_H+FONT_CH_D), bg_color);
}
/*
Перемещение указателя на следующий символ из набора
с автоматическим переносом в начало
*/
static char* pNextChar( const char *pch )
{
  // размер набора символов включая конец строки
  uint16_t size = sizeof(UPPER_CASE_SET); // 65
  // указатель на конец строки
  char* ch_max = (char*)(ch_upper + size - 1) ;
  char* pnew = (char*)pch;
  if (*pch & 0x80)
  { // символ utf8 
    if ((pnew + sizeof(wchar_t)) < ch_max) 
      pnew = pnew + sizeof(wchar_t);
    else
      pnew = (char*)ch_upper;
  }
  else
  {
    if ((pnew + 1) < ch_max)
      pnew++;
    else
      pnew = (char*)ch_upper;
  }
  return pnew;
}
/*static char* pNextChar( const char *pch )
{
  uint16_t size_l = sizeof(ch_lower);
  uint16_t size_u = sizeof(ch_upper);
  char* pnew;

  if ( (pch >= ch_lower[0]) & ( pch <= (ch_lower[0] + size_l - sizeof(ch_lower[0]))) )
  { // текущий символ - строчный
    pnew = (char*)pch + sizeof(ch_lower[0]);
    if ( pch >= (ch_lower[0] + size_l))
      pnew = (char*)ch_lower[0];
  }
  else if ( (pch >= ch_upper[0]) & ( pch <= (ch_upper[0] + size_u - sizeof(ch_upper[0]))) )
  { // текущий символ - прописной
    pnew = (char*)pch + sizeof(ch_upper[0]);
    if ( pnew >= (ch_upper[0] + size_u))
      pnew = (char*)ch_upper[0];
  //}
  return pnew;
}*/

/*
Перемещение указателя на предыдущий символ из набора
с автоматическим переносом в конец.
Алгоритм задействует перебор,
т.к. в наборе могут присутствовать символы разного размера
*/
static char* pPrevChar( const char *pch )
{
  char* pi;
  char* pnext = (char*)ch_upper;
  char* pcur = (char*)pch;
  
  if (pcur <= pnext) 
  { // если указатель в начале строки
    do
    {
      pi = pnext;
      pnext = pNextChar(pi);
    } while (pnext > pi);
  }
  else
  {
    do
    {
      pi = pnext;
      pnext = pNextChar(pi);
    } while (pnext < pcur);
  }
  
  return pi;
}
/*static char* pPrevChar( const char *pch  )
{
  uint16_t size_l = sizeof(ch_lower); // 124
  uint16_t size_u = sizeof(ch_upper); // 124
  char* pnew;
  
  if ( (pch >= ch_lower[0]) & ( pch <= (ch_lower[0] + size_l - sizeof(ch_lower[0]))) )
  { // текущий символ - строчный
    pnew = (char*)pch - sizeof(ch_lower[0]);
    if ( pch < ch_lower[0])
      pnew = (char*)ch_lower[0] + size_l - sizeof(ch_lower[0]);
  }
  else if ( (pch >= ch_upper[0]) & ( pch <= (ch_upper[0] + size_u - sizeof(ch_upper[0]))) )
  { // текущий символ - прописной
    pnew = (char*)pch - sizeof(ch_upper[0]);
    if ( pnew < ch_upper[0])
      pnew = (char*)ch_upper[0] + size_u - sizeof(ch_upper[0]);
  }
  return pnew;
}*/

static void DeInitScreen()
{
  // освобождение памяти
  
  vTaskDelete( xHandleTaskCursorBlink );
  vQueueDelete( xQueueCursor );
  
  gdispCloseFont(font);
}

static void DispAreaRst(void)
{
  gdispFillArea(screen_area.p.x, screen_area.p.y, 
                screen_area.size.x, screen_area.size.y, bg_color);
  memset( full_name, 0, sizeof(full_name) );
  
  pchar = ch_upper;
  
  char_place.x = 5;
  char_place.y = 20;
    
  DrawChar(pchar);
}
                        
// обработка событий экрана
ScreenReturnType Screen7Pool(ButtonPushType btn)
{
  wchar_t wch_buff[2];
  char *str;
  char *pfn;

  ScreenReturnType ret = SCREENx_RET_OK;
  
  switch (btn)
  {
    case BTN_DOWN:
      pchar = pNextChar(pchar);
      DrawChar(pchar);
      break;
    
    case BTN_UP:
      pchar = pPrevChar(pchar);
      DrawChar(pchar);
      break;
    
    case BTN_SET:
      str = GetStr(pchar,wch_buff);
      if  (*str == ' ')
      {
        ClearChar();                                // стереть подменный символ пробела
        CursorToNextLine(&char_place);              // смена координаты курсора
      }
      else
        CursorToNextPosition( pchar, &char_place ); // смена координаты курсора
      pfn = (char*)full_name;
      strcat(pfn, str);
      xQueueSend( xQueueCursor,  &char_place, 0);   // новое положение курсора в задачу обработки курсора
      pchar = ch_upper;   // указатель на начало набора
      DrawChar(pchar);  // вывести след символ
      break;

    case BTN_DOWN_LONG:
    case BTN_UP_LONG:
      DispAreaRst();
      break;

    case BTN_USER:
    case BTN_SET_LONG:
      GV.full_name = (char*)full_name;
      DeInitScreen();
      ret = SCREENx_RETURN;
      break;
    
    default:
      break;
  }
  
  return ret;
}

// Инициализация экрана
void Screen7Init(gColor bc, gColor c)
{
  bg_color = bc;
  color = c;
  
  char_place.x = 5;
  char_place.y = 20;
  
  font = gdispOpenFont("times18bw");
    
  xQueueCursor = xQueueCreate( 1, sizeof( char_place ) );
  MY_CHECK( xQueueCursor ) ;
    
  xTaskCreate( taskCursorBlink, "CursorBlink", 256, &char_place, 1, &xHandleTaskCursorBlink);
    
  DispAreaRst();
}

