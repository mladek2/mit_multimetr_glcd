/*
 * File:   multimeter.c
 * Author: student
 *
 * Created on 27. b?ezen 2015, 11:15
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "glcd.h"

#pragma config WDTE = OFF, PWRTE = OFF, CP = OFF, BOREN = OFF, DEBUG = OFF, LVP = ON, CPD = OFF, WRT = OFF, FOSC = XT


/*
 *
 */
unsigned char tlacitko;
unsigned short hodnota;
unsigned char x_poz=0;
unsigned char prvni[64];
unsigned char druhy[64];
unsigned char y;

unsigned short mereni_ad(unsigned char vstup) // m??ení zadaného vstupu
{
//podle tlacitka nastavit CHS2 CHS1 CHS0
    switch (vstup){
        case 0: CHS2=0;
        CHS1=0;
        CHS0=0;
        break;
        case 1: CHS2=0;
        CHS1=0;
        CHS0=1;
        break;
        case 2: CHS2=0;
        CHS1=1;
        CHS0=0;
        break;
        case 3: CHS2=0;
        CHS1=1;
        CHS0=1;
        break;
        case 4: CHS2=1;
        CHS1=0;
        CHS0=0;
        break;
    
    
    
    }




    GO_DONE=1; //zapneme p?evod
    while(GO_DONE==1);
 //v cyklu ?ekáme dokud GO_DONE=1


 //hodnota<-ADRESH:ADRESL
    hodnota=ADRESH;
    
    hodnota=(hodnota<<8)+ADRESL;
    return hodnota;
}

void zobraz(void) // zobrazení zm??eného nap?tí a ozna?ení m??eného vstupu na lcd
{
   /* unsigned char retez[5];//pole znak? ?et?z
    //do retez p?evedeme hodnota na pole znak?
    sprintf(retez,"%u",hodnota);
    lcd_write(0,0,retez);


    //vetveni dle tlacitek
    //vypí?e podle stisknutého tla?ítka TEMP,FOTO,TRIM1,TRIM2,Mikrofon výpis na ?ádek1
    switch (tlacitko){
        case 0:lcd_write_c(1,0,"TEMP ");
        break;
        case 1:lcd_write_c(1,0,"PHOTO");
        break;
        case 2:lcd_write_c(1,0,"TRIM1");
        break;
        case 3:lcd_write_c(1,0,"TRIM2");
        break;
        case 4:lcd_write_c(1,0,"MICRO");
        break;
        */

    if(x_poz<64)y=prvni[x_poz];
    else y=druhy[x_poz-64];

    glcd_del_point(x_poz,y);
    y=63-(hodnota>>4);
    glcd_point(x_poz, y);
    if(x_poz<64)prvni[x_poz]=y;
    else druhy[x_poz-64]=y;
    x_poz++;
    if(x_poz>=128){
         x_poz=0;
        // glcd_clear();
    }

}
void serial(void) // komunikace po lince USART s PC
{
    //pole znaku, pomoci sprintf v cyklu ode?leme do TXREG, dal?í odeslání po TXIF=1
    unsigned char retez[5], i;
    sprintf(retez,"%u",hodnota);
    for(i=0;i<4;i++){
        TXREG=retez[i];
        while(TXIF==0);

    }
TXREG=' ';
while(TXIF==0);
}

void vstup(void) // nastavení m??eného vstupu pomocí tla?ítek
{
    //p?epneme portD do vstupního re?imu
    TRISD=0xFF;
    NOP();
    //testujeme jednotlive piny na PORTD a cislo pinu v 0 zapí?eme do tlacitko
    if(RD0==0){tlacitko=0;}
    if(RD1==0){tlacitko=1;}
    if(RD2==0){tlacitko=2;}
    if(RD3==0){tlacitko=3;}
    if(RD4==0){tlacitko=4;}
    //p?epneme portD do výstupního re?imu
    TRISD=0;
}


void mcu_init(void)
{
    //konfigurace usart 9600, 8, bez parity   Baud Rate = FOSC/(64 (X + 1))
    TXEN=1; //povolení vysílání
    SPEN=1;//povolení seriové linky
    CREN=1;//povolení p?íjmu
    BRGH=1;//nastavení p?enosové rychlosti na 9600
    SPBRG=20;
    //zapneme AD prevodnik, prepneme portE do digitalniho rezimu
    ADFM=1;//zarovnani vpravo
    ADCS0=1;//p?epnutí-portEdo digi
    PCFG1=1;//nastavení hodin pro konverzi
    ADON=1;//zapnutí AD p?evodníku

    //vynulujeme portD a portE
    PORTD=0;
    PORTE=0;
    //prepneme portD a portE do vystupniho rezimu
    TRISD=0;
    TRISE=0;
    TRISB=0;
}

void main(void) {
    //zavolame mcu_init
    //unsigned char poc;
    mcu_init();
    glcd_init();
    glcd_clear();
    for(unsigned char i=0;i<64;i++)
    {
        prvni[i]=druhy[i]=0;
    }

    while(1)
    {
        vstup();
        mereni_ad(tlacitko);
        zobraz();
      //  serial();

    }

    return;
}


