#ifndef __DRAWICO_H
#define __DRAWICO_H

#include <stdint.h>
#include "gfx.h"
#include "screen_mng.h"

void DrawIcoDown(uint16_t x, uint16_t y, gColor c );
void DrawIcoUp(uint16_t x, uint16_t y, gColor c );

void DrawIcoDownFill(uint16_t x, uint16_t y, gColor c );
void DrawIcoUpFill(uint16_t x, uint16_t y, gColor c );

void DrawEnter(uint16_t x, uint16_t y, gColor c );

void DrawVScale(uint16_t x, uint16_t y, gColor c);
void DrawVScaleFill(uint16_t x, uint16_t y, gColor c);

void DrawHScale(uint16_t x, uint16_t y, gColor c);
void DrawHScaleFill(uint16_t x, uint16_t y, gColor c);

void DrawLeads(gPoint ref_p, uint16_t leads_mask, uint8_t lead_iactive, gColor c, gColor bc);

#endif
