#ifndef __SCREEN_MNG_H
#define __SCREEN_MNG_H

#include <stdint.h>
#include "gfx.h"
#include "my_main.h"

typedef struct Area_t
{
  gPoint p;
  gPoint size;
} Area ;

// 
typedef enum 
{
  SCREENx_RET_TO_SCREEN0,
  SCREENx_RET_TO_SCREEN1,
  SCREENx_RET_TO_SCREEN2,
  SCREENx_RET_TO_SCREEN3,
  SCREENx_RET_TO_SCREEN4,
  SCREENx_RET_TO_SCREEN5,
  SCREENx_RET_TO_SCREEN6,
  SCREENx_RET_TO_SCREEN7,
  SCREENx_RET_TO_SCREEN8,
  SCREENx_RET_TO_SCREEN9,
  SCREENx_RET_TO_SCREEN10,
  SCREENx_RET_TO_SCREEN11,
  SCREENx_RET_OK,           // saved screen state
  SCREENx_RETURN,
  SCREENx_RET_ERROR,        // undef 
} ScreenReturnType;

void ScreenInit(void);
void ScreenPool(ButtonPushType btn);

typedef void( *pScreenInitType ) (gColor bc, gColor c);
typedef ScreenReturnType ( *pScreenPoolType ) ( ButtonPushType btn );

typedef struct
{
  char* pdescription;
  pScreenInitType pScreenInit;
  pScreenPoolType pScreenPool;
} ScreenHandlersType;

typedef struct MenuType_t
{
  char* name;
  Area area;
} MenuType;

#endif
