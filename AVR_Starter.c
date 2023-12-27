//
//  Tastenblinky.c
//  Tastenblinky
//
//  Created by Sysadmin on 03.10.07.
//  Copyright Ruedi Heimlicher 2007. All rights reserved.
//



#include <avr/io.h>
#include <avr/delay.h>
//#include <avr/interrupt.h>
//#include <avr/pgmspace.h>
//#include <avr/sleep.h>
#include <inttypes.h>
//#define F_CPU 4000000UL  // 4 MHz
#include <avr/delay.h>
#include "adc.c"

#include "defines.h"
uint16_t loopCount0=0;
uint16_t loopCount1=0;
uint16_t loopCount2=0;

#define TWI_PORT		PORTC
#define TWI_PIN		PINC
#define TWI_DDR		DDRC

#define SDAPIN		4
#define SCLPIN		5

#include "lcd.h"

#define TEXTMODE
#define SSD1306 1

volatile uint8_t					Programmstatus=0x00;

extern void lcd_display(void);

void slaveinit(void)
{

	//LCD
   LOOPLED_DDR |= (1<<LOOPLED_PIN);
   //lcd_init(LCD_DISP_ON);    // init lcd and turn on
}

int main (void)
{	
	slaveinit(); // Init LoopLed
	_delay_ms(1000);
   lcd_init(LCD_DISP_ON);    // init lcd and turn on
	
   lcd_puts("Hello World");  // put string from RAM to display (TEXTMODE) or buffer (GRAPHICMODE)
   lcd_gotoxy(0,2);          // set cursor to first column at line 3
   lcd_puts_p(PSTR("String from flash"));  // puts string form flash to display (TEXTMODE) or buffer 
   
   //lcd_display();
   
   // MARK: while
	while (1) 
	{
		loopCount0 ++;
		if (loopCount0 >=0xFFFE)
		{
         loopCount0 = 0;
			//LOOPLED_PORT ^= (1<<LOOPLED_PIN);
			loopCount1++;
			
			if ((loopCount1 >0xFFFE)  )
			{
				{
					LOOPLED_PORT ^= (1<<LOOPLED_PIN);
					loopCount1=0;
				}

			}
			loopCount0 =0;
		}
	} // while
	return 0;
}
