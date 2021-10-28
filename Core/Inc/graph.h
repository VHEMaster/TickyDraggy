#include "stm32f1xx.h"
#include "lcd.h"

void graph_picture(uint8_t * data, uint16_t alpha, uint8_t trans, uint16_t x, uint16_t y);
void graph_square(uint16_t line_color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void graph_square_bg(uint16_t line_color, uint16_t bg_color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void graph_esquare(uint16_t line_color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void graph_esquare_bg(uint16_t line_color, uint16_t bg_color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void graph_print_string(uint16_t x, uint16_t y, uint16_t color, uint16_t bgcolor, uint8_t fsize, uint8_t ssize, char* _string, ...);
void graph_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void graph_line_limited(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t lim_x0, int16_t lim_y0, int16_t lim_x1, int16_t lim_y1, uint16_t color);
void graph_line_limited2(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t lim_x0, int16_t lim_y0, int16_t lim_x1, int16_t lim_y1, int16_t lim_x2, int16_t lim_y2, int16_t lim_x3, int16_t lim_y3, uint16_t color);

	


void graph_clear(void);
