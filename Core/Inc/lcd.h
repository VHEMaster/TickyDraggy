/*
 * lcd.h
 *
 *  Created on: Oct 27, 2021
 *      Author: VHEMaster
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"


#define X_SIZE      854
#define Y_SIZE      480

#define graph_command(reg) { *(volatile uint16_t *)((uint32_t)0x60000000) = reg; }
#define graph_data(value) {*(volatile uint16_t *)((uint32_t)0x60020000)=value;}

#define LCD_IO_DAT(V) {graph_data(V);}
#define LCD_IO_XY(Y,X) {graph_set_block(X,X,Y,Y);}
#define LCD_GRAM(y,x,v) {LCD_IO_XY(y,x);LCD_IO_DAT(v);}


#define graph_color(r, g, b) (((uint16_t)(r)&0xF8)<<8)+(((uint16_t)(g)&0xFC)<<3)+((uint16_t)(b)>>3)
#define graph_color_red(x) (((x)>>8)&0xF8)
#define graph_color_green(x) (((x)>>3)&0xFC)
#define graph_color_blue(x) (((x)<<3)&0xF8)


#define   COLOR_BLACK   0x0000
#define   COLOR_BLUE    0x001F
#define   COLOR_GREEN   0x07E0
#define   COLOR_CYAN    0x07FF
#define   COLOR_RED     0xF800
#define   COLOR_MAGENTA 0xF81F
#define   COLOR_YELLOW  0xFFE0
#define   COLOR_WHITE   0xFFFF
#define   COLOR_NAVY    0x000F
#define   COLOR_DGREEN  0x03E0
#define   COLOR_DCYAN   0x03EF
#define   COLOR_MAROON  0x7800
#define   COLOR_PURPLE  0x780F
#define   COLOR_OLIVE   0x7BE0
#define   COLOR_LGRAY   0xC618
#define   COLOR_DGRAY   0x7BEF

extern void graph_init(void);
extern void graph_set_block(uint16_t Xstart, uint16_t Xend, uint16_t Ystart, uint16_t Yend);

#endif /* INC_LCD_H_ */
