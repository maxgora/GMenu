#ifndef __SCREEN1_H
#define __SCREEN1_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen1Init(gColor background, gColor text);
ScreenReturnType  Screen1Pool(ButtonPushType btn);

#endif
