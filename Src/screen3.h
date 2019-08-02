#ifndef __SCREEN3_H
#define __SCREEN3_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen3Init(gColor background, gColor text);
ScreenReturnType  Screen3Pool(ButtonPushType btn);

#endif
