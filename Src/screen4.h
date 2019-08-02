#ifndef __SCREEN4_H
#define __SCREEN4_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen4Init(gColor background, gColor text);
ScreenReturnType  Screen4Pool(ButtonPushType btn);

#endif
