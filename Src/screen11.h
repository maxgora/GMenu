#ifndef __SCREEN11_H
#define __SCREEN11_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"
#include "screen_mng.h"

void              Screen11Init(gColor background, gColor text);
ScreenReturnType  Screen11Pool(ButtonPushType btn);

#endif
