#ifndef __SCREEN2_H
#define __SCREEN2_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen2Init(gColor background, gColor text);
ScreenReturnType  Screen2Pool(ButtonPushType btn);

#endif
