#include "DrawIco.h"

extern gFont font1; // малый шрифт - UI2 (h=8pxls)

extern GlobValType GV;

/*void DrawIcoUp(uint16_t x, uint16_t y, gColor c )
{
  const uint8_t size_x = 9;
  const uint8_t size_y = 13;
  
  uint8_t mx = size_x/2;
  
  gdispDrawLine((x),          (y+size_y-1), (x+mx),       (y),          c);
  gdispDrawLine((x),          (y+size_y-1), (x+size_x-1), (y+size_y-1), c);
  gdispDrawLine((x+size_x-1), (y+size_y-1), (x+mx),       (y),          c);
}*/
void DrawIcoUp(uint16_t x, uint16_t y, gColor c )
{
  const uint8_t size_x = 10;
  const uint8_t size_y = 12;
  uint8_t mx = size_x/2;
  
  point p[3] = { {(0),(size_y)},
                  {(mx),(0)},
                  {(size_x),(size_y)} };  
  gdispDrawPoly(x, y, p, 3, c);
}
void DrawIcoUpFill(uint16_t x, uint16_t y, gColor c )
{
  const uint8_t size_x = 10;
  const uint8_t size_y = 12;
  uint8_t mx = size_x/2;
  
  point p[3] = { {(0),(size_y)},
                  {(mx),(0)},
                  {(size_x),(size_y)} };  
  gdispFillConvexPoly(x,y, p, 3, c);
}

/*void DrawIcoDown(uint16_t x, uint16_t y, gColor c )
{
  const uint8_t size_x = 9;
  const uint8_t size_y = 13;
  
  uint8_t mx = size_x/2;
  
  gdispDrawLine((x),          (y),          (x+size_x-1), (y),          c);
  gdispDrawLine((x),          (y),          (x+mx),       (y+size_y-1), c);
  gdispDrawLine((x+size_x-1), (y),          (x+mx),       (y+size_y-1), c);
}*/
void DrawIcoDown(uint16_t x, uint16_t y, gColor c )
{
  const uint8_t size_x = 10;
  const uint8_t size_y = 12;
  
  uint8_t mx = size_x/2;
  
  point p[3] = { {(0),(0)},
                  {(size_x),(0)},
                  {(mx),(size_y)} };  
  gdispDrawPoly(x,y, p, 3, c);
}
void DrawIcoDownFill(uint16_t x, uint16_t y, gColor c )
{
  const uint8_t size_x = 10;
  const uint8_t size_y = 12;
  
  uint8_t mx = size_x/2;
  
  point p[3] = { {(0),(0)},
                  {(size_x),(0)},
                  {(mx),(size_y)} }; 
  gdispFillConvexPoly(x,y, p, 3, c);
}

void DrawEnter(uint16_t x, uint16_t y, gColor c )
{
  gdispDrawLine((x),          (y),          (x),          (y+8),  c);
  gdispDrawLine((x),          (y+8),        (x+17),       (y+8),  c);
  gdispDrawLine((x+11),       (y+6),        (x+17),       (y+8), c);
  gdispDrawLine((x+11),       (y+10),       (x+17),       (y+8), c);
}

void DrawVScale(uint16_t x, uint16_t y, gColor c)
{
  gdispDrawLine((11+x),(20+y),(16+x),(15+y),  c);  
  gdispDrawCircle((20+x), (11+y), 6, c); 
  point p[4] = { {0,14}, {4,0}, {8,14}, {4,28} };  
  gdispDrawPoly((x), (y), p, 4, c);
}
void DrawVScaleFill(uint16_t x, uint16_t y, gColor c)
{
  gdispDrawLine((11+x),(20+y),(16+x),(15+y),  c); 
  gdispDrawCircle((20+x), (11+y), 6, c); 
  point p[4] = { {0,14}, {4,0}, {8,14}, {4,28} };                   
  gdispFillConvexPoly((x), (y), p, 4, c);
}

void DrawHScale(uint16_t x, uint16_t y, gColor c)
{
  gdispDrawLine((25+x),(16+y),(30+x),(11+y),  c);
  gdispDrawCircle((34+x), (6+y), 6, c);  
  point p[4] = { {0,4}, {12,8}, {24,4}, {12,0} };
  gdispDrawPoly((x), (y), p, 4, c);
}
void DrawHScaleFill(uint16_t x, uint16_t y, gColor c)
{
  gdispDrawLine((25+x),(16+y),(30+x),(11+y),  c);
  gdispDrawCircle((34+x), (6+y), 6, c);   
  point p[4] = { {0,4}, {12,8}, {24,4}, {12,0} };  
  gdispFillConvexPoly((x), (y), p, 4, c);
}
/*
Вывод легенды для 10ти отведений.
Параметр:
leads_mask - маска для отведений, требующих перерисовки
*/
void DrawLeads(gPoint ref_p, uint16_t leads_mask, uint8_t lead_iactive, gColor c, gColor bc)
{
  gFont font = font1;
  gPoint txt = {ref_p.x,ref_p.y}; //{4,16};
  Area marker = {{ref_p.x+17,ref_p.y+2},{8,8}};//{{21,18},{8,8}};
  gCoord dy = 11;
  uint16_t one_sh = 1;
  int i;

  for (i = 0; i < ECG_LEADs_CNT; i++)
  { // перебор отведений
    if (one_sh & leads_mask)
    { // для i-го отведения тр-ся перерисовка легенды
      if (i == lead_iactive)
        gdispFillString(txt.x, txt.y, GV.ecg_lead[i].lead_name, font, bc, c);
      else
        gdispFillString(txt.x, txt.y, GV.ecg_lead[i].lead_name, font, c, bc);
      
      if (GV.ecg_lead[i].ecg_quality == ECG_NORMAL)
        gdispFillArea(marker.p.x, marker.p.y, marker.size.x, marker.size.y, Green);
      else
        gdispFillArea(marker.p.x, marker.p.y, marker.size.x, marker.size.y, Red);
    }
    // инк коорд у
    txt.y += dy;
    marker.p.y += dy;
    // маска для след отведения
    one_sh = one_sh << 1;
  }
}
