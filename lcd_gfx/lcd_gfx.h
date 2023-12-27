/*
 * This file is part of lcd library for ssd1306/sh1106 oled-display.
 *
 * lcd library for ssd1306/sh1106 oled-display is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any later version.
 *
 * lcd library for ssd1306/sh1106 oled-display is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von lcd library for ssd1306/sh1106 oled-display.
 *
 * lcd library for ssd1306/sh1106 oled-display ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * lcd library for ssd1306/sh1106 oled-display wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 *
 *  lcd.h
 *
 *  Created by Michael Köhler on 22.12.16.
 *  Copyright 2016 Skie-Systems. All rights reserved.
 *
 *  lib for OLED-Display with ssd1306/sh1106-Controller
 *  first dev-version only for I2C-Connection
 *  at ATMega328P like Arduino Uno
 ****************************************************
 *  For other Atmegas/Attinys: GFX-Lib needs a 
 *  minimum of 1027 byte SRAM!
 ****************************************************
 *
 */

#ifndef LCD_H
#define LCD_H

#ifndef YES 
    #define YES				1
#endif
#ifndef NO
    #define NO				0
#endif

#if (__GNUC__ * 100 + __GNUC_MINOR__) < 303
#error "This library requires AVR-GCC 3.3 or later, update to newer AVR-GCC compiler !"
#endif

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

/* TODO: define displaycontroller */
#define SSD1306	// or SH1106, check datasheet of your display

/* TODO: setup pins */
#define SDA_Pin			PC4
#define SDC_Pin			PC5
#define LCD_PORT		PORTC
#define LCD_PORT_DDR	DDRC
/* TODO: setup i2c/twi */
#define LCD_I2C_ADDR	0x78	// refer lcd-manual, 0x78 for 8-bit-adressmode, 
								//					 0x3c for 7-bit-adressmode
#define LCD_INIT_I2C	YES		// init I2C via lcd-lib
#define F_I2C			400000UL// clock i2c
#define PSC_I2C			1		// pprescaler i2c
#define SET_TWBR		(F_CPU/F_I2C-16UL)/(PSC_I2C*2UL)

#define LCD_DISP_OFF	0xAE
#define LCD_DISP_ON		0xAF

#define WHITE			0x01
#define BLACK			0x00

#define DISPLAY_WIDTH	128
#define DISPLAY_HEIGHT	64
#define DISPLAYSIZE		DISPLAY_WIDTH*DISPLAY_HEIGHT/8	// 8 pages/lines with 128 
														// 8-bit-column: 128*64 pixel
														// 1024 bytes


void i2c_init(void);						// init hw-i2c
void lcd_send_i2c_start(void);				// send i2c_start_condition
void lcd_send_i2c_stop(void);				// send i2c_stop_condition
void lcd_send_i2c_byte(uint8_t byte);		// send data_byte

void lcd_init(uint8_t dispAttr);			// init display, 
											// attributes:	LCD_DISPLAY_ON,
											//				LCD_DISPLAY_OFF
void lcd_home(void);						// set cursor to (x,y) = (0,0)

void lcd_command(uint8_t cmd[], uint8_t size);				// transmit command to display
void lcd_data(uint8_t data[], uint16_t size);				// transmit data to display
void lcd_gotoxy(uint8_t x, uint8_t y);		// set curser at pos x, y. 
											// x means character, y means line 
											// (page, refer lcd manual)
void lcd_clrscr(void);						// clear screen
void lcd_putc(char c);						// print character on screen
void lcd_puts(const char* s);				// print string, \n-terminated, from ram on screen
void lcd_puts_p(const char* progmem_s);		// print string from flash on screen
void lcd_puts_e(const char* eemem_s);       // print string form eeprom on screen
void lcd_invert(uint8_t invert);			// invert display
uint8_t lcd_isInverted(void);				// returns YES if display is inverted NO if display isn't inverted
void lcd_set_contrast(uint8_t contrast);	// set contrast for display

void lcd_drawPixel(uint8_t x, uint8_t y, uint8_t color);
void lcd_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void lcd_drawRect(uint8_t px1, uint8_t py1, uint8_t px2, uint8_t py2, uint8_t color);
void lcd_fillRect(uint8_t px1, uint8_t py1, uint8_t px2, uint8_t py2, uint8_t color);
void lcd_drawCircle(uint8_t center_x, uint8_t center_y, uint8_t radius, uint8_t color);
void lcd_fillCircle(uint8_t center_x, uint8_t center_y, uint8_t radius, uint8_t color);
void lcd_display(void);						// copy buffer to display RAM
#endif /*  LCD_H  */
