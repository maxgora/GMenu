
#include "gfx.h"
#include "main.h"
#include "pins.h"

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H
    
//#include <device.h>

/* Using FSMC A16 as RS */
#define GDISP_REG              (*((volatile uint8_t *) 0x60000000)) /* RS = 0 */
#define GDISP_RAM              (*((volatile uint8_t *) 0x60040000)) /* RS = 1 */

static GFXINLINE void init_board(GDisplay* g)
{
  // We are not using multiple displays
  //g->board = NULL;
  
  //LCD_ON();
}
static GFXINLINE void post_init_board(GDisplay* g)
{
	(void)g;
}
static GFXINLINE void setpin_reset(GDisplay *g, bool_t state)
{
	(void)g;
    
  if (state)
      LCD_RST_ON();
  else
      LCD_RST_OFF();
}
static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent)
{
	(void)g;
	(void)percent;
}
static GFXINLINE void acquire_bus(GDisplay* g)
{
	(void)g;
}
static GFXINLINE void release_bus(GDisplay* g)
{
	(void)g;
}

static GFXINLINE void write_cmd(GDisplay* g, uint8_t index)
{
	(void) g;
	GDISP_REG = index;
}

static GFXINLINE void write_data_byte(GDisplay* g, uint8_t data)
{ 
	(void) g;
	GDISP_RAM = data;
}

static GFXINLINE void write_data_word(GDisplay* g, uint16_t data)
{ 
	(void) g;
	GDISP_RAM = (uint8_t)(data>>8);
  GDISP_RAM = (uint8_t)(data);
}

static GFXINLINE void setreadmode(GDisplay* g)
{
	(void)g;
}

static GFXINLINE void setwritemode(GDisplay* g)
{
	(void)g;
}

/*static GFXINLINE uint16_t read_data(GDisplay* g)
{
	(void)g;
    
	return LCD_SPI_ReadRxData();
}*/

#endif /* _GDISP_LLD_BOARD_H */

