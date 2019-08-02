#ifndef __SCREEN0_H
#define __SCREEN0_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen0Init(gColor background, gColor text);
ScreenReturnType  Screen0Pool(ButtonPushType btn);

#endif
