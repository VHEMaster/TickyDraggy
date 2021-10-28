#include "stm32f1xx.h"
#include "lcd.h"
#include "graph.h"
#include <stdlib.h>


void graph_clear(void)
{
	uint16_t i,j;
	graph_set_block(0,X_SIZE-1,0,Y_SIZE-1);
	for(i=0;i<Y_SIZE;i++)
		for(j=0;j<X_SIZE;j++)
			graph_data(0);
}

void graph_square(uint16_t line_color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t i;
  graph_set_block(x1,x2,y1,y1);
  for(i=x1;i<=x2;i++)
    LCD_IO_DAT(line_color);
  graph_set_block(x1,x2,y2,y2);
  for(i=x1;i<=x2;i++)
    LCD_IO_DAT(line_color);

  graph_set_block(x1,x1,y1,y2);
  for(i=y1;i<=y2;i++)
    LCD_IO_DAT(line_color);
  graph_set_block(x2,x2,y1,y2);
  for(i=y1;i<=y2;i++)
    LCD_IO_DAT(line_color);

}

void graph_square_bg(uint16_t line_color, uint16_t bg_color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  uint16_t i,j;
  if(line_color == bg_color)
  {
    graph_set_block(x1,x2,y1,y2);
    for(j=y1;j<=y2;j++)
      for(i=x1;i<=x2;i++)
        LCD_IO_DAT(bg_color);
  }
  else
  {
    graph_set_block(x1,x2,y1,y1);
    for(i=x1;i<=x2;i++)
      LCD_IO_DAT(line_color);
    graph_set_block(x1,x2,y2,y2);
    for(i=x1;i<=x2;i++)
      LCD_IO_DAT(line_color);

    graph_set_block(x1,x1,y1,y2);
    for(i=y1;i<=y2;i++)
      LCD_IO_DAT(line_color);
    graph_set_block(x2,x2,y1,y2);
    for(i=y1;i<=y2;i++)
      LCD_IO_DAT(line_color);

    graph_set_block(x1+1,x2-1,y1+1,y2-1);
    for(j=y1+1;j<y2;j++)
      for(i=x1+1;i<x2;i++)
        LCD_IO_DAT(bg_color);
  }
}



void graph_esquare(uint16_t line_color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t i,xx,xx2,yy2;
	xx2 = x2-3;
	yy2 = y2-3;
	xx = x2-x1;
	for(i=x1+3;i<=xx2;i++)
	{
		LCD_GRAM(y1,i,line_color);
		LCD_GRAM(y2,i,line_color);
	}
	for(i=y1+3;i<=yy2;i++)
	{
		LCD_GRAM(i,x1,line_color);
		LCD_GRAM(i,x2,line_color);
	}

	if(xx >= 4)
	{
		LCD_GRAM(y1+1,x1+2,line_color);
		LCD_GRAM(y2-1,x1+2,line_color);
		LCD_GRAM(y1+1,x2-2,line_color);
		LCD_GRAM(y2-1,x2-2,line_color);
	}
	if(xx >= 2)
	{
		LCD_GRAM(y2-2,x2-1,line_color);
		LCD_GRAM(y1+2,x1+1,line_color);
		LCD_GRAM(y2-2,x1+1,line_color);
		LCD_GRAM(y1+2,x2-1,line_color);
	}

}

void graph_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) 
{ 
	if(x1>=x0 && y1>=y0)
	{
		if(x1-x0 >= y1-y0)
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y0,x;
			for(x=x0;x<=x1;x++)
			{
					LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x0,y;
			for(y=y0;y<=y1;y++)
			{
					LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
	else if(x0>=x1 && y0>=y1)
	{
		if(x0-x1 >= y0-y1)
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y1,x;
			for(x=x1;x<=x0;x++)
			{
					LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x1,y;
			for(y=y1;y<=y0;y++)
			{
					LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
	else if(x0>=x1 && y1>=y0)
	{
		if(x0-x1 >= y1-y0)
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y0,x;
			for(x=x0;x>=x1;x--)
			{
					LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x1,y;
			for(y=y1;y>=y0;y--)
			{
					LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
	else if(x1>=x0 && y0>=y1)
	{
		if(x1-x0 >= y0-y1)
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y1,x;
			for(x=x1;x>=x0;x--)
			{
					LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x0,y;
			for(y=y0;y>=y1;y--)
			{
					LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
	
}


void graph_line_limited2(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t lim_x0, int16_t lim_y0, int16_t lim_x1, int16_t lim_y1, int16_t lim_x2, int16_t lim_y2, int16_t lim_x3, int16_t lim_y3, uint16_t color)
{
	if(x1>=x0 && y1>=y0)
	{
		if(x1-x0 >= y1-y0)
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y0,x;
			for(x=x0;x<=x1;x++)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1)
						if(x < lim_x2 && x > lim_x3 && y < lim_y2 && y > lim_y3) 
							LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x0,y;
			for(y=y0;y<=y1;y++)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1)
						if(x < lim_x2 && x > lim_x3 && y < lim_y2 && y > lim_y3) 
							LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
	else if(x0>=x1 && y0>=y1)
	{
		if(x0-x1 >= y0-y1)
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y1,x;
			for(x=x1;x<=x0;x++)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1)
						if(x < lim_x2 && x > lim_x3 && y < lim_y2 && y > lim_y3) 
							LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x1,y;
			for(y=y1;y<=y0;y++)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1)
						if(x < lim_x2 && x > lim_x3 && y < lim_y2 && y > lim_y3) 
							LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
	else if(x0>=x1 && y1>=y0)
	{
		if(x0-x1 >= y1-y0)
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y0,x;
			for(x=x0;x>=x1;x--)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1)
						if(x < lim_x2 && x > lim_x3 && y < lim_y2 && y > lim_y3) 
							LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x1,y;
			for(y=y1;y>=y0;y--)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1)
						if(x < lim_x2 && x > lim_x3 && y < lim_y2 && y > lim_y3) 
							LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
	else if(x1>=x0 && y0>=y1)
	{
		if(x1-x0 >= y0-y1)
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y1,x;
			for(x=x1;x>=x0;x--)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1)
						if(x < lim_x2 && x > lim_x3 && y < lim_y2 && y > lim_y3) 
							LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x0,y;
			for(y=y0;y>=y1;y--)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1)
						if(x < lim_x2 && x > lim_x3 && y < lim_y2 && y > lim_y3) 
							LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
}

void graph_line_limited(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t lim_x0, int16_t lim_y0, int16_t lim_x1, int16_t lim_y1, uint16_t color)
{ 
	if(x1>=x0 && y1>=y0)
	{
		if(x1-x0 >= y1-y0)
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y0,x;
			for(x=x0;x<=x1;x++)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1) LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x0,y;
			for(y=y0;y<=y1;y++)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1) LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
	else if(x0>=x1 && y0>=y1)
	{
		if(x0-x1 >= y0-y1)
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y1,x;
			for(x=x1;x<=x0;x++)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1) LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x1,y;
			for(y=y1;y<=y0;y++)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1) LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
	else if(x0>=x1 && y1>=y0)
	{
		if(x0-x1 >= y1-y0)
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y0,x;
			for(x=x0;x>=x1;x--)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1) LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x0 - x1);
			int16_t deltay = abs(y1 - y0);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x1,y;
			for(y=y1;y>=y0;y--)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1) LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
	else if(x1>=x0 && y0>=y1)
	{
		if(x1-x0 >= y0-y1)
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltay;
			int16_t y = y1,x;
			for(x=x1;x>=x0;x--)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1) LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltax)
					{
							y = y + 1;
							error = error - deltax;	
					}
			}
		}
		else
		{
			int16_t deltax = abs(x1 - x0);
			int16_t deltay = abs(y0 - y1);
			int16_t error = 0;
			int16_t deltaerr = deltax;
			int16_t x = x0,y;
			for(y=y0;y>=y1;y--)
			{
					if(x >= lim_x0 && x <= lim_x1 && y >= lim_y0 && y <= lim_y1) LCD_GRAM(y,x,color);
					error = error+deltaerr;
					if(2*error >= deltay)
					{
							x = x + 1;
							error = error - deltay;	
					}
			}
		}
	}
	
}



