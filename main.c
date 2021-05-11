
#include <msp430.h>
#include <stdio.h>
#include "ini.h"
#include "timer.h"
#include "in_out.h"
#include "lcd_helper.h"
#include "lcd_driver.h"
#include "izpisi_LCD.h"

#define BUFFER_SIZE 60

char key_pressed;
unsigned int i;

char buffer_1[BUFFER_SIZE];
int read_pointer_1 = 0;
int write_pointer_1 = 0;

int state = 0;
char counter = 0;
//-----------------------------------------------------------------------------
unsigned int value = 0000;

char lcd_text[32] =      MEASURED_TEXT;
char lcd_text_wait[32] = MEASURMENT_IN_PROGRESS;
char *lcd_string;

//-----------------------------------------------------------------------------

void sleep_1s()
{
  for(int i=0; i<1000; i++){
    __delay_cycles( DELAY_1ms );
  }
}

int inc_pointer(char pointer_RW, char pointer_buffer, int increment)
{
  switch(pointer_RW)
  {
  case 'r':
      read_pointer_1 += increment;
      if (read_pointer_1>=BUFFER_SIZE){
        read_pointer_1 -= BUFFER_SIZE;}
      return read_pointer_1;
      break;
  case 'w':
      write_pointer_1 += increment;
      if (write_pointer_1>=BUFFER_SIZE){
        write_pointer_1 -= BUFFER_SIZE;}
      return write_pointer_1;
      break;
  default:
      break;
  }
  return 0;
}

int buffer_size()
{
  int size = (BUFFER_SIZE+write_pointer_1-read_pointer_1)%BUFFER_SIZE;
  return size;
  
}

int main( void )
{
  WDTCTL = WDTPW + WDTHOLD;// Stop watchdog timer to prevent time out reset

  //Define variables
  time_10ms = 0;
  time_1ms = 0;
  time_seconds = 0;
  time_start = 0;
  task_status = STATUS_DEFAULT;
  opravilo = TASK_INIT;
  
  //Init system
  init_pins();
  Set_Clk();
  DAC_init();
  init_timer(); 
  
  //Init and start AC
  DAC_init_AC();  
  DAC_run_50();
    
  P1OUT &= ~( AMP_GAIN_SELECT_0 + AMP_GAIN_SELECT_1 ); // pobrisi prejsnje nastavitve
  P1OUT |=  AMP_GAIN_ENABLE;
  
  //Init UART
  
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
  DCOCTL = CALDCO_16MHZ; 
  P3SEL = 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 0x82;                              // 1MHz 115200
  UCA0BR1 = 0x6;                              // 1MHz 115200
  UCA0MCTL = 0x92;                           // Modulation UCBRSx = 5
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

  
 
  lcd_init(); 
  lcd_write_string(lcd_text_wait);
  lcd_string = lcd_text;
  __bis_SR_register( GIE); // enable interrupts
  //sleep_1s();
  
  //__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
  //sleep_1s();
  int sucess_count = 0xFFFF;
  char counter_error = 0;
  char tem[32];
  sprintf(tem, "%02X:%02X:%02X:%02X:%02X  %02X:%02X:%02X:%02X:%02X  ",0,0,0,0,0,0,0,0,0,0);
  char output[32];
  int pos_out = 0;
  int old_values[5];
  while(1)
  {     
    int available_characters = buffer_size();
    if (available_characters>30)
    {  
      char read_char = '0';
      while (read_char!=0x78)
      {
        read_char = buffer_1[inc_pointer('r', 1, 1)];
      }
      char new_counter = buffer_1[inc_pointer('r', 1, 1)];
      if (counter+1==new_counter || (counter==0xFF && new_counter==0x00))  //Chasing bits. Could work without it, but it's fine now. 
      {
        counter=new_counter;
        sucess_count++;
        sprintf(output, "%05X:%05X:%02X  %05X:%05X:%02X  ",sucess_count,old_values[0],counter,old_values[1],old_values[2],old_values[4]);
       
        //sprintf(output, tem, read_char, new_counter, counter, sucess_count);
        lcd_write_string(output);
        inc_pointer('r', 1, 27);
      }
      else
      {
        counter_error = counter-new_counter;
        old_values[4]=new_counter;
        counter=new_counter;
        //inc_pointer('r', 1, 2);        
        old_values[pos_out]=sucess_count;
        pos_out++;
        if(pos_out>3)
        {pos_out = 0;}
        sucess_count = 0;
        //char output[32];
        //sprintf(output, "%02X:%02X:%02X:%02X--   %02X:%02X:%02X:%02X:%02X  ", read_char, new_counter, counter, available_characters, buffer_1[inc_pointer('r', 1, 1)], buffer_1[inc_pointer('r', 1, 1)], buffer_1[inc_pointer('r', 1, 1)], buffer_1[inc_pointer('r', 1, 1)],buffer_1[inc_pointer('r', 1, 1)]);
        //lcd_write_string(output);
      }
      
    }
    
    
    /*
    int available_characters = (BUFFER_SIZE+write_pointer_1-read_pointer_1)%BUFFER_SIZE;
    if (available_characters>60)
    {
      if(state == 0)
      {
        char read_char = buffer_1[inc_pointer('r', 1, 1)];
        if(read_char==0x78)
        {
          int new_counter = buffer_1[inc_pointer('r', 1, 1)];
              number_of_windings(lcd_text, new_counter, 0);
              
              measured_mV(lcd_text, counter, 0);
              
              number_of_windings(lcd_text, write_pointer_1-read_pointer_1, 1);
              lcd_write_string(lcd_text);
          if (counter+1 == new_counter)
          {     
            counter++; // = new_counter;
            measured_mV(lcd_text, counter, 1);
            lcd_write_string(lcd_text);
            //state = 1;            
            inc_pointer('r', 1, 20);
          }
          else
          {
            state = 0;
          }
        }
      }
      if (state == 1)
      {
        char read_char = buffer_1[inc_pointer('r', 1, 1)];
        if(read_char==0x78)
        {
          int new_counter = buffer_1[inc_pointer('r', 1, 1)];
          if (counter+1 == new_counter)
          { 
            counter = new_counter;
            if (counter%4==0)
            {
              inc_pointer('r', 1, 4);
              int Urms = buffer_1[inc_pointer('r', 1, 1)] * 0XFF / 44 + buffer_1[inc_pointer('r', 1, 1)] / 44;
              inc_pointer('r', 1, 12);
              int UDC = buffer_1[inc_pointer('r', 1, 1)] * 0XFF / 44 + buffer_1[inc_pointer('r', 1, 1)] / 44;
              
              number_of_windings(lcd_text, new_counter, 0);
              measured_mV(lcd_text, Urms, 0);
          
              number_of_windings(lcd_text, counter, 1);
              measured_mV(lcd_text, UDC, 1);
              lcd_write_string(lcd_text);
            }
            
          }
          else
          {
            state = 0;
          }
        }
      }
    }
      
      */
      
      
    if( time_200ms_flag )
    {
      time_200ms_flag = 0; // clear 200 ms flag
      /*value++;
      
      number_of_windings(lcd_text, 1300, 0);
      measured_mV(lcd_text, value, 0);
  
      number_of_windings(lcd_text, 1100, 1);
      measured_mV(lcd_text, value+1000, 1);
      lcd_write_string(lcd_text);
      
      */
      
      if( opravilo > TASK_INIT )
      {
        if( P2IN & KEY_START ) // key START releasedd. Chamber opened??????????????????????????????
        {
          task_status = STATUS_ERROR;
          opravilo = TASK_FAIL;
          lcd_write_string(DEVICE_OPENED);
        } 
      }
     
      //=======================================================
      // INIT
      //=======================================================
      if( opravilo & TASK_INIT ) 
      {
        if( ( P1IN & KEY_START ) == 0 ) // key START pressed
        {
          P1OUT &= ~( CORE1_LED_GREEN | CORE1_LED_RED | CORE2_LED_GREEN | CORE2_LED_RED );// LED off 
          
          P1OUT &= ~( AMP_GAIN_SELECT_0 + AMP_GAIN_SELECT_1 ); // pobrisi prejsnje nastavitve za oja�anje
          
          if( KEY_SELECT_CORE )
          {
            P1OUT |= AMP_GAIN_31mA; // s stikalom smo izbrali testiranje JEDRO 208 ( kontrolni tok 31 mA )
          }
          else
          {
            P1OUT |= AMP_GAIN_65mA; // s stikalom smo izbrali testiranje JEDRO NEW ( kontrolni tok 65,5 mA )
          }
        }
        else
        {
          
        }
      }
      //=======================================================     
      // TASK_50Hz :
      //=======================================================
      if( opravilo & TASK_50Hz )
      {
        //test_50Hz();
      }
      //=======================================================     
      // TASK_DC :
      //=======================================================
      if( opravilo & TASK_DC )
      {
        //test_DC();
      }
      
      //=======================================================
      // TEST_FINISHED:
      //=======================================================
      
      if( opravilo_finished >= ( TASK_POWER_ON | TASK_50Hz | TASK_DC ) )
      {
        if( 1 )
        {
            opravilo = TASK_INIT; // stikalo se je razklenilo, naprej na zacetek
            
            opravilo_finished = 0;
            
            task_status = STATUS_DEFAULT;
        }
      }
      
      //=======================================================
      // TASK_FAIL:
      //=======================================================
      if( opravilo & TASK_FAIL )
      {
        opravilo = TASK_FAIL;
        
        if( 0 ) // elektronika se je vzela ven, pojdi na zacetek
        // Dead code!
        {
          opravilo = TASK_INIT;
          task_status = STATUS_DEFAULT;
        }
      }
    }
  }   
}

// Echo back RXed character, confirm TX buffer is ready first

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)

{
  while (!(IFG2&UCA0TXIFG));
  UCA0TXBUF = 'U';
  buffer_1[inc_pointer('w', 1, 1)] = UCA0RXBUF;
  if(write_pointer_1==read_pointer_1)
    {write_pointer_1--;}
}
