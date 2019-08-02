#ifndef __SCREEN5_H
#define __SCREEN5_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen5Init(gColor background, gColor text);
ScreenReturnType  Screen5Pool(ButtonPushType btn);

#endif
