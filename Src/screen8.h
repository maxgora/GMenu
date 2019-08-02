#ifndef __SCREEN8_H
#define __SCREEN8_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen8Init(gColor background, gColor text);
ScreenReturnType  Screen8Pool(ButtonPushType btn);

#endif
