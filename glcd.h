/*
 * File:   glcd.c
 * Author: Uzivatel
 *
 * Created on 18. ?íjen 2014, 19:59
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "font5x7.h"

//#pragma config WDTE = OFF, PWRTE = OFF, CP = OFF, BOREN = OFF, DEBUG = OFF, LVP = ON, CPD = OFF, WRT = OFF, FOSC = XT

#define _XTAL_FREQ  3276800

#define IN  0xFF
#define OUT  0x00

#define	GLCD_PORT	PORTD
#define GLCD_PORT_TRIS  TRISD

#define GLCD_RS	RE0//PORTEbits.RE0
#define GLCD_RW	RE1//PORTEbits.RE1
#define GLCD_EN	RE2//PORTEbits.RE2

#define GLCD_CS1	RB0//PORTAbits.RA0
#define GLCD_CS2	RB1//PORTAbits.RA1

#define GLCD_RESETB     RB2//PORTAbits.RA2

#define RED_LED     RB3

#define GLCD_MAX_X  128
#define GLCD_MAX_Y  64

#define GLCD_DISPLAY_ON_OFF 0b00111110 //1..ON, 0..OFF
#define GLCD_SET_ADDRESS    0b01000000
#define GLCD_SET_PAGE       0b10111000
#define GLCD_DISPLAY_START_LINE 0b11000000

//eeprom char ascii[];
//const unsigned char font5x7[] @ 0x1E00;



//void glcd_set_page(unsigned char page);
//void glcd_set_col(unsigned char col);
//void glcd_get_col(unsigned char col);
//void glcd_cmd_write(const unsigned char x);
//unsigned char glcd_read(void);

void glcd_cmd_write(const unsigned char cmd)
{
    GLCD_PORT_TRIS = OUT;
    NOP();

    GLCD_RS = GLCD_RW = 0;
    NOP();

    GLCD_EN = 1;
    NOP();

    GLCD_PORT = cmd;
    NOP();

    GLCD_EN = 0;
    NOP();

    __delay_us(5);
}

void glcd_data_write(const unsigned char data)
{
    GLCD_PORT_TRIS = OUT;
    NOP();

    GLCD_RS = 1;
    GLCD_RW = 0;
    NOP();

    GLCD_EN = 1;
    NOP();

    GLCD_PORT = data;
    NOP();

    GLCD_EN = 0;
    NOP();

    __delay_us(5);
}

unsigned char glcd_data_read(void)
{
    unsigned char out;

    GLCD_PORT_TRIS = IN;

	GLCD_RS = 1;
        GLCD_RW = 1;
        NOP();

	GLCD_EN = 1;
        NOP();

	GLCD_EN = 0;
        NOP();

        GLCD_EN = 1;
        NOP();

        out = GLCD_PORT;

        GLCD_EN = 0;
        NOP();

	__delay_us(5);

        return out;
}

void glcd_del_point(unsigned char x, unsigned char y)
{
    unsigned char y_page, y_bit, col;
    if (!((x < GLCD_MAX_X) && (y < GLCD_MAX_Y))) return;
    if (x < (GLCD_MAX_X >> 1))
    {
        GLCD_CS1 = 1;
        GLCD_CS2 = 0;
    }
    else
        {
        GLCD_CS1 = 0;
        GLCD_CS2 = 1;
        x = x - (GLCD_MAX_X >> 1);
    }
    y_page = y / 8;
    y_bit = y % 8;
    glcd_cmd_write(GLCD_SET_PAGE + y_page);
    glcd_cmd_write(GLCD_SET_ADDRESS + x);
    col = glcd_data_read();
    col = col & ~(1 << y_bit);
    glcd_cmd_write(GLCD_SET_ADDRESS + x);
    glcd_data_write(col);
}

void glcd_point(unsigned char x, unsigned char y)
{
    unsigned char y_page, y_bit, col;
    if (!((x < GLCD_MAX_X) && (y < GLCD_MAX_Y))) return;
    if (x < (GLCD_MAX_X >> 1))
    {
        GLCD_CS1 = 1;
        GLCD_CS2 = 0;
    }
    else
        {
        GLCD_CS1 = 0;
        GLCD_CS2 = 1;
        x = x - (GLCD_MAX_X >> 1);
    }
    y_page = y / 8;
    y_bit = y % 8;
    glcd_cmd_write(GLCD_SET_PAGE + y_page);
    glcd_cmd_write(GLCD_SET_ADDRESS + x);
    col = glcd_data_read();
    col = col | (1 << y_bit);
    glcd_cmd_write(GLCD_SET_ADDRESS + x);
    glcd_data_write(col);
}
/*void glcd_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{ unsigned char pom_x, pom_y,pom_max,pom_pom,i,a;
    float souradnice_x,souradnice_y;
    if(x1>x2){pom_x=x2-x1;
   a=x1;x1=x2;x2=a;
    }else{pom_x=x1-x2;}
    if(y1>y2){pom_y=y2-y1;
   a=y1;y1=y2;y2=a;
    }else{pom_y=y1-y2;}
    if(pom_x>pom_y){
        souradnice_y=pom_x/pom_y;
        for(i=0;i<pom_x;i++){
            y1+=souradnice_y;
            x1++;
             glcd_point(x1,y1);
        }}else{
        souradnice_x=pom_y/pom_x;
        for(i=0;i<pom_y;i++){
            x1+=souradnice_x;
            y1++;
             glcd_point(x1,y1);

        }


    }




}*/


void glcd_line(int x0, int y0, int x1, int y1) {

  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = (dx>dy ? dx : -dy)/2, e2;

  for(;;){
    glcd_point(x0,y0);
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}


/*void glcd_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
    unsigned char pom_x, pom_y,pom_max,pom_pom,i,a;
    float souradnice_x,souradnice_y;
    if(x1>x2){pom_x=x2-x1;
    a=x1;
    x1=x2;x2=a;
    }else{ pom_x=x1-x2;}
    if(y1>y2){pom_y=y2-y1;
     a=y1;
    y1=y2;y2=a;
    }else{ pom_y=y1-y2;}

    if(pom_x>pom_y){
        pom_max=pom_x;
        pom_pom=1;
    }else{
    pom_max=pom_y;
     pom_pom=0;
    }
    if(pom_pom==1){
       souradnice_y=pom_y/pom_x;
        for(i=0;i<pom_x;i++){
                        y1+=souradnice_y;

            if(x1<x2){
            souradnice_x=x1+i;
            }else{souradnice_x=x2+i;}
            glcd_point((unsigned char)souradnice_x,(y1));
        }
    }else{
    souradnice_x=pom_x/pom_y;
        for(i=0;i<pom_y;i++){
            x1+=souradnice_x;
                if(y1<y2){
            souradnice_y=y1+i;
            }else{souradnice_y=y2+i;}
          // souradnice_y=y1+i;
            //souradnice_x=souradnice_x*i;
            glcd_point(x1,(unsigned char)souradnice_y);
        }


    }

}
*/
/*void glcd_line(int x1, int y1, int x2, int y2) {
 int dx,dy,d,d10,d11,i;
  dx=x2-x1;   dy=y2-y1;  d=2*dy-dx;  d10=2*dy; d11=2*dy-2*dx;
  for (i=0; i<=dx; i++)
    { __delay_ms(10);
      glcd_point(x1,y1);
      if (d>0)
         { x1++; y1++; d+=d11; }
      else
         { x1++; d+=d10; }
    }
}*/

void glcd_clear(void)
{
    unsigned char i, j;

    GLCD_CS1 = GLCD_CS2 = 1;

    for (i = 0; i < 8; i++)
    {
        glcd_cmd_write(GLCD_SET_PAGE + i);
        for (j = 0; j < 64; j++)
        {
            glcd_data_write(0x00);
        }
    }
}

void glcd_fill(unsigned char in)
{
    unsigned char i, j;

    GLCD_CS1 = GLCD_CS2 = 1;

    for (i = 0; i < 8; i++)
    {
        glcd_cmd_write(GLCD_SET_PAGE + i);
        for (j = 0; j < 64; j++)
        {
            glcd_data_write(in);
        }
    }
}

void glcd_init(void)
{
    //reset samostatne pro oba radice, min delka 1ms
    //E min 450ns
    GLCD_RESETB = 1;
    GLCD_EN = 1;

    __delay_ms(250);

    GLCD_CS1 = 1;
    GLCD_CS2 = 0;

    glcd_cmd_write(GLCD_DISPLAY_ON_OFF | 0b1);
    glcd_cmd_write(GLCD_SET_ADDRESS);
    glcd_cmd_write(GLCD_SET_PAGE);
    glcd_cmd_write(GLCD_DISPLAY_START_LINE);

    GLCD_CS1 = 0;
    GLCD_CS2 = 1;

    glcd_cmd_write(GLCD_DISPLAY_ON_OFF | 0b1);
    glcd_cmd_write(GLCD_SET_ADDRESS);
    glcd_cmd_write(GLCD_SET_PAGE);
    glcd_cmd_write(GLCD_DISPLAY_START_LINE);
}



/*
 *
 */
