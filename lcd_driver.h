// Date:   21.6.2011
// Author: Andrej Vidensek

#define DELAY_50us  50*8
#define DELAY_1ms   1000*8
#define DELAY_41ms  41000*8


// LCD pin masks (MSP 430)

#define DB4   BIT0 // P3.0
#define DB5   BIT1 // P3.1 	
#define DB6   BIT2 // P3.2	
#define DB7   BIT3 // P3.3	

#define RS    BIT5 // P3.5
#define RW    BIT6 // P3.6
#define E     BIT7 // P3.7



// LCD commands
#define CLEAR_DISPLAY			0x01
#define RETURN_HOME			    0x02
#define MODE				    0x04
#define CURSOR_DIRECTION_RIGHT	0x02	// MODE command flag
#define CURSOR_DIRECTION_LEFT	0x00	// MODE command flag
#define DISPLAY_SHIFT_ON		0x01	// MODE command flag
#define DISPLAY_SHIFT_OFF		0x00	// MODE command flag
#define CONTROL				    0x08
#define DISPLAY_ON			    0x04	// CONTROL command flag
#define DISPLAY_OFF			    0x00	// CONTROL command flag
#define CURSOR_ON			    0x02	// CONTROL command flag
#define CURSOR_OFF			    0x00	// CONTROL command flag
#define BLINK_ON			    0x01	// CONTROL command flag
#define BLINK_OFF			    0x00	// CONTROL command flag
#define SHIFT				    0x10
#define DISPLAY				    0x08	// SHIFT command flag
#define CURSOR				    0x00	// SHIFT command flag
#define RIGHT				    0x04	// SHIFT command flag
#define LEFT				    0x00	// SHIFT command flag
#define SET				        0x20
#define BITS_8				    0x10	// SET command flag
#define BITS_4				    0x00	// SET command flag
#define LINES_2				    0x08	// SET command flag
#define LINE_1				    0x00	// SET command flag
#define DOTS_5_10			    0x04	// SET command flag
#define DOTS_5_8			    0x00	// SET command flag
#define CGRAM				    0x40
#define DDRAM				    0x80

// Function declarations

extern void lcd_write_comm(int com);
extern void lcd_write_data(int data);
extern void lcd_init();
