/********************************************************
*   LCD HD44780 driver helper                           *
*   2x16, 4 bit mode (DB4-DB7),                         *
*                                                       *      
*   Includes functions to write to and from LCD         *
*   Author: Matej Cigale                                *
*********************************************************/

#include "lcd_driver.h"
#include "lcd_helper.h"

void number_of_windings(char* lcd_text, int value, int line)
/******************************************************************************
 * Generate the number of vindings string in an appropriate format for output.*
 ******************************************************************************/
{  
   int line_offset = line*16;
   lcd_text[WINDINGS+line_offset+0] = ( value/1000)%10 + '0';
   lcd_text[WINDINGS+line_offset +1] = ( value/100 )%10 + '0';
   lcd_text[WINDINGS+line_offset +2] = ( value/10  )%10 + '0';
   lcd_text[WINDINGS+line_offset +3] = ( value%10) + '0';
}

void measured_mV(char* lcd_text, int value, int line)
/******************************************************************************
 * Generate the mV string in an appropriate format for output.*
 ******************************************************************************/
{   
    int line_offset = line*16;
    lcd_text[MV+line_offset+0] = ( value/1000)%10 + '0';
    lcd_text[MV+line_offset+1] = '.';
    lcd_text[MV+line_offset+2] = ( value/100 )%10 + '0';
    lcd_text[MV+line_offset+3] = ( value/10  )%10 + '0';
    lcd_text[MV+line_offset+4] = ( value%10) + '0';
}

void lcd_write_string(char* lcd_string)
{
  int i;
  
  lcd_write_comm(DDRAM | 0x00);
  for(i = 0; i < 16; i++) lcd_write_data(lcd_string[i]);
  
  lcd_write_comm(DDRAM | 0x40);
  for(; i < 32; i++) lcd_write_data(lcd_string[i]);
}