/*
 * lcd.c
 *
 *  Created on: Oct 27, 2021
 *      Author: VHEMaster
 */


#include "lcd.h"
#include "graph.h"
#include "RREFont.h"

#define graph_command(reg) { *(volatile uint16_t *)((uint32_t)0x60000000) = reg; }
#define graph_data(value) {*(volatile uint16_t *)((uint32_t)0x60020000)=value;}


void lcd_rect_solid(int x, int y, int w, int h, int c)
{
  graph_set_block(x, x+w-1, y, y+h-1);

  for(int i = y, ii = 0; ii < h; ii++, i++)
  {
    for(int j = x, jj = 0; jj < w; jj++, j++)
    {
      graph_data(c);
    }
  }
}

inline void graph_set_block(uint16_t Xstart, uint16_t Xend, uint16_t Ystart, uint16_t Yend)
{
  graph_command(0x2a);
  graph_data(Xstart>>8);
  graph_data(Xstart&0xff);
  graph_data(Xend>>8);
  graph_data(Xend&0xff);

  graph_command(0x2b);
  graph_data(Ystart>>8);
  graph_data(Ystart&0xff);
  graph_data(Yend>>8);
  graph_data(Yend&0xff);

  graph_command(0x2c);
}


void graph_init(void)
{

  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(20);

  //************* Start Initial Sequence **********//
  graph_command(0xFF); // EXTC Command Set enable register
  graph_data(0xFF);
  graph_data(0x98);
  graph_data(0x06);

  graph_command(0xBA); // SPI Interface Setting
  graph_data(0xE0);

  graph_command(0xBC); // GIP 1
  graph_data(0x03);
  graph_data(0x0F);
  graph_data(0x63);
  graph_data(0x69);
  graph_data(0x01);
  graph_data(0x01);
  graph_data(0x1B);
  graph_data(0x11);
  graph_data(0x70);
  graph_data(0x73);
  graph_data(0xFF);
  graph_data(0xFF);
  graph_data(0x08);
  graph_data(0x09);
  graph_data(0x05);
  graph_data(0x00);
  graph_data(0xEE);
  graph_data(0xE2);
  graph_data(0x01);
  graph_data(0x00);
  graph_data(0xC1);

  graph_command(0xBD); // GIP 2
  graph_data(0x01);
  graph_data(0x23);
  graph_data(0x45);
  graph_data(0x67);
  graph_data(0x01);
  graph_data(0x23);
  graph_data(0x45);
  graph_data(0x67);

  graph_command(0xBE); // GIP 3
  graph_data(0x00);
  graph_data(0x22);
  graph_data(0x27);
  graph_data(0x6A);
  graph_data(0xBC);
  graph_data(0xD8);
  graph_data(0x92);
  graph_data(0x22);
  graph_data(0x22);

  graph_command(0xC7); // Vcom
  graph_data(0x1E);

  graph_command(0xED); // EN_volt_reg
  graph_data(0x7F);
  graph_data(0x0F);
  graph_data(0x00);

  graph_command(0xC0); // Power Control 1
  graph_data(0xE3);
  graph_data(0x0B);
  graph_data(0x00);

  graph_command(0xFC);
  graph_data(0x08);

  graph_command(0xDF); // Engineering Setting
  graph_data(0x00);
  graph_data(0x00);
  graph_data(0x00);
  graph_data(0x00);
  graph_data(0x00);
  graph_data(0x02);

  graph_command(0xF3); // DVDD Voltage Setting
  graph_data(0x74);

  graph_command(0xB4); // Display Inversion Control
  graph_data(0x00);
  graph_data(0x00);
  graph_data(0x00);

  graph_command(0xF7); // 480x854
  graph_data(0x81);

  graph_command(0xB1); // Frame Rate
  graph_data(0x00);
  graph_data(0x10);
  graph_data(0x14);

  graph_command(0xF1); // Panel Timing Control
  graph_data(0x29);
  graph_data(0x8A);
  graph_data(0x07);

  graph_command(0xF2); //Panel Timing Control
  graph_data(0x40);
  graph_data(0xD2);
  graph_data(0x50);
  graph_data(0x28);

  graph_command(0xC1); // Power Control 2
  graph_data(0x17);
  graph_data(0X85);
  graph_data(0x85);
  graph_data(0x20);

  graph_command(0xE0);
  graph_data(0x00); //P1
  graph_data(0x0C); //P2
  graph_data(0x15); //P3
  graph_data(0x0D); //P4
  graph_data(0x0F); //P5
  graph_data(0x0C); //P6
  graph_data(0x07); //P7
  graph_data(0x05); //P8
  graph_data(0x07); //P9
  graph_data(0x0B); //P10
  graph_data(0x10); //P11
  graph_data(0x10); //P12
  graph_data(0x0D); //P13
  graph_data(0x17); //P14
  graph_data(0x0F); //P15
  graph_data(0x00); //P16

  graph_command(0xE1);
  graph_data(0x00); //P1
  graph_data(0x0D); //P2
  graph_data(0x15); //P3
  graph_data(0x0E); //P4
  graph_data(0x10); //P5
  graph_data(0x0D); //P6
  graph_data(0x08); //P7
  graph_data(0x06); //P8
  graph_data(0x07); //P9
  graph_data(0x0C); //P10
  graph_data(0x11); //P11
  graph_data(0x11); //P12
  graph_data(0x0E); //P13
  graph_data(0x17); //P14
  graph_data(0x0F); //P15
  graph_data(0x00); //P16

  graph_command(0x35); //Tearing Effect ON
  graph_data(0x00);

  graph_command(0x11); //Exit Sleep
  HAL_Delay(120);
  graph_command(0x29); // Display On
  HAL_Delay(10);

  graph_command(0x3A); graph_data(0x55);
  graph_command(0x36); graph_data(0x60);

  graph_set_block(0,853,0,479);

  for(int i = 0; i < X_SIZE*Y_SIZE; i++)
    graph_data(0);

  font_init(lcd_rect_solid, X_SIZE, Y_SIZE);
  font_setCR(0);

  graph_clear();


}


