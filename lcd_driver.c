/********************************************************
*   LCD HD44780 driver
*   2x16, 4 bit mode (DB4-DB7),
*
*   Driver fro the LCD
********************************************************/

#include <msp430.h>
#include "lcd_driver.h"
#include "in430.h"
#include "intrinsics.h"

void busy()
{
  int busy_flag;
  
  P2DIR &= ~( DB4 | DB5 | DB6 | DB7 ); // DB4,5,6,7 as input
  P2DIR |= ( E | RW | RS ); // as output
  P2OUT &= ~RS; // set LOW
  P2OUT |= RW; // set HIGH
  do
  {
    __delay_cycles( DELAY_50us );
    P2OUT |= E; // set HIGH
    __delay_cycles( DELAY_50us );
    busy_flag = P2IN & DB7; // value of DB7
    
    P2OUT &= ~E; // set LOW
    __delay_cycles( DELAY_50us );
    
    P2OUT |= E;  // set HIGH
    __delay_cycles( DELAY_50us );
    
    P2OUT &= ~E; // set LOW
  } 
  while( busy_flag );
  
  P2DIR |= ( DB4 | DB5 | DB6 | DB7 ); // set as OUTPUT
}

// LCD write high four bits
// byte ... data or command byte
void lcd_write_high(int byte)
{
  P2OUT &= ~RW; // set LOW
  
  __delay_cycles( DELAY_50us );
  
  P2OUT &= ~( DB4 | DB5 | DB6 | DB7); // set LOW
  
  P2OUT |= E; // set HIGH
    
  if( byte & 0x80) P2OUT |= DB7;
  if( byte & 0x40) P2OUT |= DB6;
  if( byte & 0x20) P2OUT |= DB5;
  if( byte & 0x10) P2OUT |= DB4;
  
  __delay_cycles( DELAY_50us );
  
  P2OUT &= ~E; // set LOW
}

// LCD write low four bits
// byte ... data or command byte
void lcd_write_low(int byte)
{
  __delay_cycles( DELAY_50us );
  
  P2OUT &= ~(DB6 | DB5 | DB4 | DB7 ); // set LOW
    
  P2OUT |= E; // set HIGH
  
  //****///
  
  if( byte & 0x08) P2OUT |= DB7;
  if( byte & 0x04) P2OUT |= DB6;
  if( byte & 0x02) P2OUT |= DB5;
  if( byte & 0x01) P2OUT |= DB4;
  
  //P4OUT |= ( (byte << 4) & (DB7 | DB6 | DB5 | DB4) ); // send LOW NIBBLE
  
  //****///
        
  __delay_cycles( DELAY_50us );
  
  P2OUT &= ~E; // set LOW
  busy();
}

// LCD write half command
// com ... command
void lcd_write_half_comm(int com)
{
  // IO0CLR = RS;
  P2OUT &= ~RS;
  lcd_write_high(com);
}

// LCD write command
// com ... command
void lcd_write_comm(int com)
{
  lcd_write_half_comm(com);
  lcd_write_low(com);
}

// LCD write data
// data ... character
void lcd_write_data(int data)
{
  P2OUT |= RS; // set HIGH
  
  lcd_write_high(data);
  lcd_write_low(data);
}

// LCD initialisation routine
void lcd_init()
{
 
  P2DIR |= ( RS | RW | E | DB7 | DB6 | DB5 | DB4 ); // set as OUTPUT
                     
  P2OUT &= ~E; // set LOW
  
  __delay_cycles( DELAY_41ms );
  __delay_cycles( DELAY_41ms );
  
  //__delay_cycles(200000);
  //__delay_cycles(200000);
  
  lcd_write_half_comm( SET | BITS_8 );
  
  __delay_cycles( DELAY_41ms );
  
  lcd_write_half_comm( SET | BITS_8 );
  
  __delay_cycles( DELAY_1ms );
  
  lcd_write_half_comm( SET | BITS_8 );
  busy();
	
  lcd_write_half_comm( SET | BITS_4 );
	
  busy();
	
  lcd_write_comm( SET | BITS_4 | LINES_2 | DOTS_5_8 );
	
  lcd_write_comm( CONTROL | DISPLAY_OFF | CURSOR_OFF | BLINK_OFF );
	
  lcd_write_comm( CLEAR_DISPLAY );
	
  lcd_write_comm( MODE | CURSOR_DIRECTION_RIGHT | DISPLAY_SHIFT_OFF );
	
  lcd_write_comm( CONTROL | DISPLAY_ON | CURSOR_OFF | BLINK_OFF );
}


