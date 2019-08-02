#ifndef __SCREEN9_H
#define __SCREEN9_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen9Init(gColor background, gColor text);
ScreenReturnType  Screen9Pool(ButtonPushType btn);

#endif
