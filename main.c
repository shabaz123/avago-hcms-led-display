
#include <stdio.h>
#include <stdlib.h>
#include "BBBIOlib/BBBio_lib/BBBiolib.h"
#include "font.h"

// wiring header P9
// VLED(5V)  5
// VLOGIC(3.3V) 3
// GND 1

// CE_PIN 15
// RS_PIN 13
// BL_PIN 23
// RESET_PIN 16
// CLK_PIN 14
// DIN_PIN 27

#define CE_PIN 9,15
#define RS_PIN 9,12
#define BL_PIN 9,23
#define RESET_PIN 9,16
#define CLK_PIN 9,14
#define DIN_PIN 8,12

#define NUM_MODULES 3
#define DELAY ;

unsigned char vram[60];

void
vram_print_char(int idx,char c)
{
  int i, vidx, fontidx;

  vidx=idx*5;
  fontidx=(((int)c)-0x20)*5;

  for (i=0; i<5; i++)
  {
    vram[vidx++]=font[fontidx++];
  }
}

void
vram_print_string(int idx, char* s)
{
  char* ptr;
  int i;

  i=idx;
  ptr=s;
  do
  {
    vram_print_char(i, *ptr);
    i++;
    ptr++;
    if (i>11) break;
  } while (*ptr != 0);
}  

int
main(int argc, char* argv[])
{
  int i, j, k;
  unsigned char cw[2];
  unsigned char cb, vbyte;
  cw[0]=0x7f; // 0x7f = Display enabled, max brightnessa
  cw[1]=0x80; // 0x80 = Serial mode
  iolib_init();


  iolib_setdir(9,15, BBBIO_DIR_OUT);
  iolib_setdir(9,12, BBBIO_DIR_OUT);
  iolib_setdir(9,23, BBBIO_DIR_OUT);
  iolib_setdir(9,16, BBBIO_DIR_OUT);
  iolib_setdir(9,14, BBBIO_DIR_OUT);
  iolib_setdir(8,12, BBBIO_DIR_OUT);

  // zeroise the video ram
  for (i=0; i<60; i++)
  {
      vram[i]=0;
  }

  if (argc==1)
  {
    vram_print_string(0, "No parameter");
  }
  else if (argc>1)
  {
    vram_print_string(0, argv[1]);
  }


  // reset the LED display
  pin_high(CE_PIN);
  pin_low(BL_PIN);
  pin_low(CLK_PIN);
  pin_low(RS_PIN);
  pin_low(RESET_PIN);
  iolib_delay_ms(100);
  DELAY;
  pin_high(RESET_PIN);
  DELAY;

  // load zeroes into the dot registers
  pin_low(RS_PIN);
  DELAY;
  pin_low(CE_PIN);
  pin_low(DIN_PIN);
  DELAY;
  for (i=0; i<(NUM_MODULES*160); i++)
  {
    pin_high(CLK_PIN);
    DELAY;
    if (i==((NUM_MODULES*160)-1))
    {
      pin_high(CE_PIN);
    }
    pin_low(CLK_PIN);
    DELAY;
  }

  // select Control Register
  
  for (k=0; k<2; k++)
  {
    pin_high(RS_PIN);
    DELAY;
    pin_low(CE_PIN);
    DELAY;

    for (j=0; j<NUM_MODULES; j++)
    {
      cb=cw[k];
      for (i=0; i<8; i++)
      {
        if (cb & 0x80)
        {
          pin_high(DIN_PIN);
        }
        else
        {
          pin_low(DIN_PIN);
        }
        cb=cb<<1;
        DELAY;
        pin_high(CLK_PIN);
        DELAY;
        if ((j==(NUM_MODULES-1)) && (i==7))
        {
          pin_high(CE_PIN);
        }
        pin_low(CLK_PIN);
        DELAY;
      }
    }
  }

      
  // load the pattern into the dot registers
  pin_low(RS_PIN);
  DELAY;
  pin_low(CE_PIN);
  j=0; k=7;
  for (i=0; i<(NUM_MODULES*20); i++)
  {
    vbyte=vram[i];
    for (j=0; j<8; j++)
    {
      if (vbyte & 0x80)
      {
        pin_high(DIN_PIN);
      }
      else
      {
        pin_low(DIN_PIN);
      }
      vbyte=vbyte<<1;

      pin_high(CLK_PIN);
      DELAY;
      if ((i==((NUM_MODULES*20)-1)) && (j>6))
      {
        pin_high(CE_PIN);
      }
      pin_low(CLK_PIN);
      DELAY;
    }
  }

  iolib_free();
  return(0);
}


