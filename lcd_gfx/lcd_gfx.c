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
/* Standard ASCII 6x8 font */

#include "lcd_gfx.h"
#include <math.h>
#include <stdlib.h>

#if PSC_I2C != 1 && PSC_I2C != 4 && PSC_I2C != 16 && PSC_I2C != 64
#error "Wrong prescaler for TWI at ATMega328p!"
#elif SET_TWBR < 0 || SET_TWBR > 255
#error "TWBR out of range, change PSC_I2C or F_I2C !"
#endif

static uint8_t displayBuffer[DISPLAYSIZE];
uint8_t displayIsInverted = NO;
uint16_t actualIndex = 0;

static const uint8_t ssd1306oled_font6x8 [] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // sp
    0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, // !
    0x00, 0x00, 0x07, 0x00, 0x07, 0x00, // "
    0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14, // #
    0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12, // $
    0x00, 0x62, 0x64, 0x08, 0x13, 0x23, // %
    0x00, 0x36, 0x49, 0x55, 0x22, 0x50, // &
    0x00, 0x00, 0x05, 0x03, 0x00, 0x00, // '
    0x00, 0x00, 0x1c, 0x22, 0x41, 0x00, // (
    0x00, 0x00, 0x41, 0x22, 0x1c, 0x00, // )
    0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, // *
    0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, // +
    0x00, 0x00, 0x00, 0xA0, 0x60, 0x00, // ,
    0x00, 0x08, 0x08, 0x08, 0x08, 0x08, // -
    0x00, 0x00, 0x60, 0x60, 0x00, 0x00, // .
    0x00, 0x20, 0x10, 0x08, 0x04, 0x02, // /
    0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
    0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, // 1
    0x00, 0x42, 0x61, 0x51, 0x49, 0x46, // 2
    0x00, 0x21, 0x41, 0x45, 0x4B, 0x31, // 3
    0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, // 4
    0x00, 0x27, 0x45, 0x45, 0x45, 0x39, // 5
    0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
    0x00, 0x01, 0x71, 0x09, 0x05, 0x03, // 7
    0x00, 0x36, 0x49, 0x49, 0x49, 0x36, // 8
    0x00, 0x06, 0x49, 0x49, 0x29, 0x1E, // 9
    0x00, 0x00, 0x36, 0x36, 0x00, 0x00, // :
    0x00, 0x00, 0x56, 0x36, 0x00, 0x00, // ;
    0x00, 0x08, 0x14, 0x22, 0x41, 0x00, // <
    0x00, 0x14, 0x14, 0x14, 0x14, 0x14, // =
    0x00, 0x00, 0x41, 0x22, 0x14, 0x08, // >
    0x00, 0x02, 0x01, 0x51, 0x09, 0x06, // ?
    0x00, 0x32, 0x49, 0x59, 0x51, 0x3E, // @
    0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C, // A
    0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, // B
    0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, // C
    0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C, // D
    0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, // E
    0x00, 0x7F, 0x09, 0x09, 0x09, 0x01, // F
    0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A, // G
    0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, // H
    0x00, 0x00, 0x41, 0x7F, 0x41, 0x00, // I
    0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, // J
    0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, // K
    0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, // L
    0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
    0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F, // N
    0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, // O
    0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, // P
    0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
    0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, // R
    0x00, 0x46, 0x49, 0x49, 0x49, 0x31, // S
    0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, // T
    0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F, // U
    0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F, // V
    0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F, // W
    0x00, 0x63, 0x14, 0x08, 0x14, 0x63, // X
    0x00, 0x07, 0x08, 0x70, 0x08, 0x07, // Y
    0x00, 0x61, 0x51, 0x49, 0x45, 0x43, // Z
    0x00, 0x00, 0x7F, 0x41, 0x41, 0x00, // [
    0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55, // backslash
    0x00, 0x00, 0x41, 0x41, 0x7F, 0x00, // ]
    0x00, 0x04, 0x02, 0x01, 0x02, 0x04, // ^
    0x00, 0x40, 0x40, 0x40, 0x40, 0x40, // _
    0x00, 0x00, 0x01, 0x02, 0x04, 0x00, // '
    0x00, 0x20, 0x54, 0x54, 0x54, 0x78, // a
    0x00, 0x7F, 0x48, 0x44, 0x44, 0x38, // b
    0x00, 0x38, 0x44, 0x44, 0x44, 0x20, // c
    0x00, 0x38, 0x44, 0x44, 0x48, 0x7F, // d
    0x00, 0x38, 0x54, 0x54, 0x54, 0x18, // e
    0x00, 0x08, 0x7E, 0x09, 0x01, 0x02, // f
    0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C, // g
    0x00, 0x7F, 0x08, 0x04, 0x04, 0x78, // h
    0x00, 0x00, 0x44, 0x7D, 0x40, 0x00, // i
    0x00, 0x40, 0x80, 0x84, 0x7D, 0x00, // j
    0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, // k
    0x00, 0x00, 0x41, 0x7F, 0x40, 0x00, // l
    0x00, 0x7C, 0x04, 0x18, 0x04, 0x78, // m
    0x00, 0x7C, 0x08, 0x04, 0x04, 0x78, // n
    0x00, 0x38, 0x44, 0x44, 0x44, 0x38, // o
    0x00, 0xFC, 0x24, 0x24, 0x24, 0x18, // p
    0x00, 0x18, 0x24, 0x24, 0x18, 0xFC, // q
    0x00, 0x7C, 0x08, 0x04, 0x04, 0x08, // r
    0x00, 0x48, 0x54, 0x54, 0x54, 0x20, // s
    0x00, 0x04, 0x3F, 0x44, 0x40, 0x20, // t
    0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C, // u
    0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C, // v
    0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C, // w
    0x00, 0x44, 0x28, 0x10, 0x28, 0x44, // x
    0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C, // y
    0x00, 0x44, 0x64, 0x54, 0x4C, 0x44, // z
    0x00, 0x00, 0x08, 0x77, 0x41, 0x00, // {
    0x00, 0x00, 0x00, 0x63, 0x00, 0x00, // ¦
    0x00, 0x00, 0x41, 0x77, 0x08, 0x00, // }
    0x00, 0x08, 0x04, 0x08, 0x08, 0x04, // ~
    0x00, 0x3D, 0x40, 0x40, 0x20, 0x7D, // ü
    0x00, 0x3D, 0x40, 0x40, 0x40, 0x3D, // Ü
    0x00, 0x21, 0x54, 0x54, 0x54, 0x79, // ä
    0x00, 0x7D, 0x12, 0x11, 0x12, 0x7D, // Ä
    0x00, 0x39, 0x44, 0x44, 0x44, 0x39, // ö
    0x00, 0x3D, 0x42, 0x42, 0x42, 0x3D, // Ö
    0x00, 0x02, 0x05, 0x02, 0x00, 0x00, // °
    0x00, 0x7E, 0x01, 0x49, 0x55, 0x73, // ß
};

const uint8_t init_sequence [] PROGMEM = {	// Initialization Sequence
    0xAE,			// Display OFF (sleep mode)
    0x20, 0b00,		// Set Memory Addressing Mode
				// 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
				// 10=Page Addressing Mode (RESET); 11=Invalid
    0xB0,			// Set Page Start Address for Page Addressing Mode, 0-7
    0xC8,			// Set COM Output Scan Direction
    0x00,			// --set low column address
    0x10,			// --set high column address
    0x40,			// --set start line address
    0x81, 0x3F,		// Set contrast control register
    0xA1,			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
    0xA6,			// Set display mode. A6=Normal; A7=Inverse
    0xA8, 0x3F,		// Set multiplex ratio(1 to 64)
    0xA4,			// Output RAM to Display
				// 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
    0xD3, 0x00,		// Set display offset. 00 = no offset
    0xD5,			// --set display clock divide ratio/oscillator frequency
    0xF0,			// --set divide ratio
    0xD9, 0x22,		// Set pre-charge period
    0xDA, 0x12,		// Set com pins hardware configuration
    0xDB,			// --set vcomh
    0x20,			// 0x20,0.77xVcc
    0x8D, 0x14,		// Set DC-DC enable
    
};
#if defined (__AVR_ATmega328P__)
void i2c_init(void){
    LCD_PORT |= (1 << SDA_Pin)|(1 << SDC_Pin);		// experimental, pullups
    LCD_PORT_DDR |= (1 << SDA_Pin)|(1 << SDC_Pin);	// on for 12c_bus
    // set clock
    switch (PSC_I2C) {
        case 4:
            TWSR = 0x1;
            break;
        case 16:
            TWSR = 0x2;
            break;
        case 64:
            TWSR = 0x3;
            break;
        default:
            TWSR = 0x00;
            break;
    }
    TWBR = (uint8_t)SET_TWBR;
    // enable
    TWCR = (1 << TWEN);
}
void lcd_send_i2c_start(void){
    // i2c start
    TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
    while((TWCR & (1 << TWINT)) == 0);
    // send adress
    TWDR = LCD_I2C_ADDR;
    TWCR = (1 << TWINT)|( 1 << TWEN);
    while((TWCR & (1 << TWINT)) == 0);
}
void lcd_send_i2c_stop(void){
    // i2c stop
    TWCR = (1 << TWINT)|(1 << TWSTO)|(1 << TWEN);
}
void lcd_send_i2c_byte(uint8_t byte){
    TWDR = byte;
    TWCR = (1 << TWINT)|( 1 << TWEN);
    while((TWCR & (1 << TWINT)) == 0);
}

void lcd_init(uint8_t dispAttr){
    if(LCD_INIT_I2C == YES) i2c_init();
    uint8_t commandSequence[sizeof(init_sequence)+1];
    for (uint8_t i = 0; i < sizeof (init_sequence); i++) {
        commandSequence[i] = (pgm_read_byte(&init_sequence[i]));
    }
    commandSequence[sizeof(init_sequence)]=(dispAttr);
    lcd_command(commandSequence, sizeof(commandSequence));
    lcd_clrscr();
}
#endif
void lcd_home(void){
    lcd_gotoxy(0, 0);
}

void lcd_command(uint8_t cmd[], uint8_t size) {
    lcd_send_i2c_start();
    lcd_send_i2c_byte(0x00);	// 0x00 for command, 0x40 for data
    for (uint8_t i=0; i<size; i++) {
        lcd_send_i2c_byte(cmd[i]);
    }
    lcd_send_i2c_stop();
}
void lcd_data(uint8_t data[], uint16_t size) {
    lcd_send_i2c_start();
    lcd_send_i2c_byte(0x40);	// 0x00 for command, 0x40 for data
    for (uint16_t i=0; i<size; i++) {
        lcd_send_i2c_byte(data[i]);
    }
    lcd_send_i2c_stop();
}
void lcd_gotoxy(uint8_t x, uint8_t y){
    if( x > (DISPLAY_WIDTH/6) || y > (DISPLAY_HEIGHT/8-1)) return;// out of display
    x = x * 6;					// one char: 6 pixel width
    actualIndex = (x)+(y*DISPLAY_WIDTH);
#if defined SSD1306
    uint8_t commandSequence[] = {0xb0+y, 0x21, x, 0x7f};
#elif defined SH1106
    uint8_t commandSequence[] = {0xb0+y, 0x21, (0x00+((2+x) & (0x0f))), (0x10+( ((2+x) & (0xf0)) >> 4 )), 0x7f};
#endif
    lcd_command(commandSequence, sizeof(commandSequence));
}
void lcd_clrscr(void){
    for (uint16_t i=0; i<sizeof(displayBuffer); i++) {
        displayBuffer[i] = 0x00;
    }
#if defined SSD1306
    lcd_data(displayBuffer, sizeof(displayBuffer));
#elif defined SH1106
    for (uint8_t i=0; i <= DISPLAY_HEIGHT/8; i++) {
        uint8_t actualLine[DISPLAY_WIDTH];
        for (uint8_t j=0; j<DISPLAY_WIDTH; j++) {
            actualLine[j]=displayBuffer[i*DISPLAY_WIDTH+j];
        }
        lcd_data(actualLine, sizeof(actualLine));
        lcd_gotoxy(0, i);
    }
#endif
    lcd_home();
}
void lcd_putc(char c){
    if((c > 127 ||
        c < 32) &&
       (c != 'ü' &&
        c != 'ö' &&
        c != '°' &&
        c != 'ä' &&
        c != 'ß' &&
        c != 'Ü' &&
        c != 'Ö' &&
        c != 'Ä' ) ) return;
    switch (c) {
        case 'ü':
            c = 95; // ü - 188
            for (uint8_t i = 0; i < 6; i++)
            {
                displayBuffer[actualIndex+i] = pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]);	// print font to ram, print 6 columns
            }
            break;
        case 'ö':
            c = 99; // ö
            for (uint8_t i = 0; i < 6; i++)
            {
                displayBuffer[actualIndex+i] = pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]);	// print font to ram, print 6 columns
            }
            break;
        case '°':
            c = 101; // °
            for (uint8_t i = 0; i < 6; i++)
            {
                displayBuffer[actualIndex+i] = pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]);	// print font to ram, print 6 columns
            }
            break;
        case 'ä':
            c = 97; // ä
            for (uint8_t i = 0; i < 6; i++)
            {
                displayBuffer[actualIndex+i] = pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]);	// print font to ram, print 6 columns
            }
            break;
        case 'ß':
            c = 102; // ß
            for (uint8_t i = 0; i < 6; i++)
            {
                displayBuffer[actualIndex+i] = pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]);	// print font to ram, print 6 columns
            }
            break;
        case 'Ü':
            c = 96; // Ü
            for (uint8_t i = 0; i < 6; i++)
            {
                displayBuffer[actualIndex+i] = pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]);	// print font to ram, print 6 columns
            }
            break;
        case 'Ö':
            c = 100; // Ö
            for (uint8_t i = 0; i < 6; i++)
            {
                displayBuffer[actualIndex+i] = pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]);	// print font to ram, print 6 columns
            }
            break;
        case 'Ä':
            c = 98; // Ä
            for (uint8_t i = 0; i < 6; i++)
            {
                displayBuffer[actualIndex+i] = pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]);	// print font to ram, print 6 columns
            }
            break;
        default:
            c -= 32;
            for (uint8_t i = 0; i < 6; i++)
            {
                displayBuffer[actualIndex+i] = pgm_read_byte(&ssd1306oled_font6x8[c * 6 + i]);
            }
            break;
    }
    actualIndex += 6;
}
void lcd_puts(const char* s){
    while (*s) {
        lcd_putc(*s++);
    }
}
void lcd_puts_p(const char* progmem_s){
    register uint8_t c;
    while ((c = pgm_read_byte(progmem_s++))) {
        lcd_putc(c);
    }
}
void lcd_puts_e(const char* eemem_s){
    register uint8_t c;
    while ((c = eeprom_read_byte((uint8_t*)eemem_s++))) {
        lcd_putc(c);
    }
}
void lcd_invert(uint8_t invert){
    displayIsInverted = invert;
    uint8_t commandSequence[1];
    if (invert == YES) {
        commandSequence[0] = 0xA7;
    } else {
        commandSequence[0] = 0xA6;
    }
    lcd_command(commandSequence, 1);
}
uint8_t lcd_isInverted(void){
    return displayIsInverted;
}
void lcd_set_contrast(uint8_t contrast){
    uint8_t commandSequence[2] = {0x81, contrast};
    lcd_command(commandSequence, sizeof(commandSequence));
}

void lcd_drawPixel(uint8_t x, uint8_t y, uint8_t color){
    if( x > DISPLAY_WIDTH-1 || y > (DISPLAY_HEIGHT-1)) return; // out of Display
    if( color == WHITE){
        displayBuffer[(uint8_t)(y / (DISPLAY_HEIGHT/8)) * DISPLAY_WIDTH +x] |= (1 << (y % (DISPLAY_HEIGHT/8)));
    } else {
        displayBuffer[(uint8_t)(y / (DISPLAY_HEIGHT/8)) * DISPLAY_WIDTH +x] &= ~(1 << (y % (DISPLAY_HEIGHT/8)));
    }
}
void lcd_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color){
    if( ((x1 || x2) > DISPLAY_WIDTH-1) ||
       ((y1 || y2) > DISPLAY_HEIGHT-1) ) return;
    int dx =  abs(x2-x1), sx = x1<x2 ? 1 : -1;
    int dy = -abs(y2-y1), sy = y1<y2 ? 1 : -1;
    int err = dx+dy, e2; /* error value e_xy */
    
    while(1){
        lcd_drawPixel(x1, y1, color);
        if (x1==x2 && y1==y2) break;
        e2 = 2*err;
        if (e2 > dy) { err += dy; x1 += sx; } /* e_xy+e_x > 0 */
        if (e2 < dx) { err += dx; y1 += sy; } /* e_xy+e_y < 0 */
    }
}

void lcd_drawRect(uint8_t px1, uint8_t py1, uint8_t px2, uint8_t py2, uint8_t color){
    if( ((px1 || px2) > DISPLAY_WIDTH-1) ||
       ((py1 || py2) > DISPLAY_HEIGHT-1) ) return;
    lcd_drawLine(px1, py1, px2, py1, color);
    lcd_drawLine(px2, py1, px2, py2, color);
    lcd_drawLine(px2, py2, px1, py2, color);
    lcd_drawLine(px1, py2, px1, py1, color);
}
void lcd_fillRect(uint8_t px1, uint8_t py1, uint8_t px2, uint8_t py2, uint8_t color){
    if( px1 > px2){
        uint8_t temp = px1;
        px1 = px2;
        px2 = temp;
        temp = py1;
        py1 = py2;
        py2 = temp;
    }
    for (uint8_t i=0; i<=(py2-py1); i++){
        lcd_drawLine(px1, py1+i, px2, py1+i, color);
    }
}
void lcd_drawCircle(uint8_t center_x, uint8_t center_y, uint8_t radius, uint8_t color){
    if( ((center_x - radius) > DISPLAY_WIDTH-1) ||
       ((center_y - radius) > DISPLAY_HEIGHT-1) ||
       center_x < radius ||
       center_y < radius) return;
    int16_t f = 1 - radius;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    int16_t x = 0;
    int16_t y = radius;
    
    lcd_drawPixel(center_x  , center_y+radius, color);
    lcd_drawPixel(center_x  , center_y-radius, color);
    lcd_drawPixel(center_x+radius, center_y  , color);
    lcd_drawPixel(center_x-radius, center_y  , color);
    
    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        lcd_drawPixel(center_x + x, center_y + y, color);
        lcd_drawPixel(center_x - x, center_y + y, color);
        lcd_drawPixel(center_x + x, center_y - y, color);
        lcd_drawPixel(center_x - x, center_y - y, color);
        lcd_drawPixel(center_x + y, center_y + x, color);
        lcd_drawPixel(center_x - y, center_y + x, color);
        lcd_drawPixel(center_x + y, center_y - x, color);
        lcd_drawPixel(center_x - y, center_y - x, color);
    }
}
void lcd_fillCircle(uint8_t center_x, uint8_t center_y, uint8_t radius, uint8_t color) {
    for(uint8_t i=0; i<= radius;i++){
        lcd_drawCircle(center_x, center_y, i, color);
    }
}
void lcd_display() {
    lcd_gotoxy(0,0);
#if defined SSD1306
    lcd_data(displayBuffer, sizeof(displayBuffer));
#elif defined SH1106
    for (uint8_t i=0; i < DISPLAY_HEIGHT/8; i++) {
        lcd_gotoxy(0, i);
        uint8_t actualLine[DISPLAY_WIDTH];
        for (uint8_t j=0; j< DISPLAY_WIDTH; j++) {
            actualLine[j]=displayBuffer[i*DISPLAY_WIDTH+j];
        }
        lcd_data(actualLine, sizeof(actualLine));
        
    }
#endif
}
