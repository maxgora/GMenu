#ifndef __SCREEN7_H
#define __SCREEN7_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen7Init(gColor background, gColor text);
ScreenReturnType  Screen7Pool(ButtonPushType btn);

#endif
