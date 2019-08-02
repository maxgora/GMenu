#ifndef __HEADER_H
#define __HEADER_H

#include <stdint.h>
#include "my_main.h"
#include "gfx.h"

void HeaderInit(gColor background, gColor text);
void HeaderBtUpdate(SignalStateType state);
void HeaderAccumUpdate(int8_t percent);
void HeaderTimeUpdate(void);

#endif
