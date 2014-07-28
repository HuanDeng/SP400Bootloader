
#include "stm32f10x.h"


/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define lcdWIDTH  ( 240 )
#define lcdHEIGHT ( 320 )

extern void lcdInit(void);
extern void LCD_Ascii_One(uint8_t x, uint16_t y, char Char, uint16_t Color);
extern void showstring(unsigned char x, unsigned char y,u16 fcolor,u16 bcolor,char *str);
extern void lcdClear();