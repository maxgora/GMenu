#ifndef __SCREEN6_H
#define __SCREEN6_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen6Init(gColor background, gColor text);
ScreenReturnType  Screen6Pool(ButtonPushType btn);

#endif
