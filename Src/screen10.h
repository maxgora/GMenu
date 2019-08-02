#ifndef __SCREEN10_H
#define __SCREEN10_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen10Init(gColor background, gColor text);
ScreenReturnType  Screen10Pool(ButtonPushType btn);

#endif
