
#include <msp430.h>
#include <stdio.h>
#include "ini.h"
#include "timer.h"
#include "in_out.h"
#include "lcd_helper.h"
#include "lcd_driver.h"
#include "izpisi_LCD.h"

#define BUFFER_SIZE 60
#define AVERING_NUM 10
#define ERROR_FIX 1.2
#define NUMBER_WINDINGS_HI 1300
#define NUMBER_WINDINGS_LO 1100
#define LIMIT_HI 1650
#define LIMIT_LO 1500

char key_pressed;
unsigned int i;

int averaging_table[2][AVERING_NUM][2];
int av_pointer[2] = {0,0}; 

char buffer[2][BUFFER_SIZE];
int read_pointer[2] = {0,0};
int write_pointer[2] = {0,0};

int state[2] = {0,0};
char counter[2] = {0,0};
//-----------------------------------------------------------------------------
unsigned int value = 0000;

char lcd_text[32] =      MEASURED_TEXT;
char lcd_text_wait[32] = MEASURMENT_IN_PROGRESS;
char *lcd_string;

//-----------------------------------------------------------------------------


int* averaging(int buf, int RMS, int UDS)
{
  if (UDS > 400 && UDS < 6000)
  {
    averaging_table[buf][av_pointer[buf]][0] = RMS;
    averaging_table[buf][av_pointer[buf]][1] = UDS;
    av_pointer[buf]++;
    if (av_pointer[buf]>=AVERING_NUM)
      av_pointer[buf] = 0;
  }
  unsigned int sum[2] = {0,0};
  for (int i=0; i<AVERING_NUM; i++)
  {
    sum[0] += averaging_table[buf][i][0];
    sum[1] += averaging_table[buf][i][1]; 
  }
  static int out[2] = {0,0};
  out[0] = sum[0]/AVERING_NUM;
  out[1] = sum[1]/AVERING_NUM;
  return out;
}

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
      read_pointer[pointer_buffer] += increment;
      if (read_pointer[pointer_buffer]>=BUFFER_SIZE){
        read_pointer[pointer_buffer] -= BUFFER_SIZE;}
      return read_pointer[pointer_buffer];
      break;
  case 'w':
      write_pointer[pointer_buffer] += increment;
      if (write_pointer[pointer_buffer]>=BUFFER_SIZE){
        write_pointer[pointer_buffer] -= BUFFER_SIZE;}
      return write_pointer[pointer_buffer];
      break;
  default:
      break;
  }
  return 0;
}

int buffer_size(char buf)
{
  int size = (BUFFER_SIZE+write_pointer[buf]-read_pointer[buf])%BUFFER_SIZE;
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
  //So this is mostly Copy-paste and values form an online generator
  //i.e. I do not understand any of it :D
  
  DCOCTL = 0;                              
  BCSCTL1 = CALBC1_16MHZ;                   
  DCOCTL = CALDCO_16MHZ; 
  P3SEL = 0xF0;       
  
  UCA0CTL1 |= UCSSEL_2;                     
  UCA0BR0 = 0x82;                           
  UCA0BR1 = 0x6;                              
  UCA0MCTL = 0x92;                         
  UCA0CTL1 &= ~UCSWRST;                   
  IE2 |= UCA0RXIE;     
  
  
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  //P3SEL = 0xC0;                             // P3.6,7 = USCI_A1 TXD/RXD
  UCA1CTL1 |= UCSSEL_2;                     // CLK = ACLK
  UCA1BR0 = 0x82;                           // 2400 (see User's Guide)
  UCA1BR1 = 0x6;                           //
  UCA1MCTL = 0x92;                       // Modulation UCBRSx = 3
  UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UC1IE |= UCA1RXIE;                        // Enable USCI_A1 RX interrupt
 
  lcd_init(); 
  lcd_write_string(lcd_text_wait);
  lcd_string = lcd_text;
  __bis_SR_register( GIE); // enable interrupts
  //sleep_1s();
  
  //__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
  //sleep_1s();
  char tem[32];
  sprintf(tem, "%02X:%02X:%02X:%02X:%02X  %02X:%02X:%02X:%02X:%02X  ",0,0,0,0,0,0,0,0,0,0);
  while(1)
  {  
    for(int buf = 0; buf<2; buf++)
    {
      int available_characters = (BUFFER_SIZE+write_pointer[buf]-read_pointer[buf])%BUFFER_SIZE;
      if (available_characters>40)
      {
        if(state[buf] == 0)
        {
          char read_char = '0';
          while (read_char!=0x78)
          {
            read_char = buffer[buf][inc_pointer('r', buf, 1)];
          }
          char new_counter = buffer[buf][inc_pointer('r', buf, 1)];
          if (counter[buf]+1 == new_counter || (counter[buf]==0xFF && new_counter==0x00))
          {     
            counter[buf] = new_counter;
            state[buf] = 1;            
            inc_pointer('r', buf, 27);
          }
          else
          {
            
            //measured_mV(lcd_text, counter[buf], 1);
            //lcd_write_string(lcd_text);
            counter[buf] = new_counter;
            state[buf] = 0;
          }
        }
        if (state[buf] == 1)
        {
          char read_char = buffer[buf][inc_pointer('r', buf, 1)];
          if(read_char==0x78)
          {
            int new_counter = buffer[buf][inc_pointer('r', buf, 1)];
            if (counter[buf]+1 == new_counter)
            { 
              counter[buf] = new_counter;
              if ((counter[buf]%4)==0)
              {
                /*
                char buff_out[8];
                for(int i=0; i<8; i++)
                {
                  buff_out[i] = buffer_1[inc_pointer('r', 1, 1)];
                }
                sprintf(tem, "%02X:%02X:%02X:%02X:%02X  %02X:%02X:%02X:%02X:%02X  ",read_char,new_counter,buff_out[0],buff_out[1],buff_out[2],buff_out[3],buff_out[4],buff_out[5],buff_out[6],buff_out[7]);
                lcd_write_string(tem);*/
                
                inc_pointer('r', buf, 4);
                int Urms = ((buffer[buf][inc_pointer('r', buf, 1)] + buffer[buf][inc_pointer('r', buf, 1)] * 0xFF) / 4.4 * ERROR_FIX);
                inc_pointer('r', buf, 12);
                int UDC = ((buffer[buf][inc_pointer('r', buf, 1)] + buffer[buf][inc_pointer('r', buf, 1)]* 0xFF) / 4.4);
                
                int Av_T[2];
                
                int *avereged_V=Av_T;
                avereged_V = averaging(buf, Urms, UDC);
                measured_mV(lcd_text, avereged_V[0], buf);
                
                int num_windings = 0;
                if (avereged_V[0]<LIMIT_LO || avereged_V[0]>LIMIT_HI)
                  num_windings = NUMBER_WINDINGS_HI;
                else
                  num_windings = NUMBER_WINDINGS_LO;
                
                number_of_windings(lcd_text, num_windings, buf);
                lcd_write_string(lcd_text);
              }
              
            }
            else
            {
              counter[buf] = new_counter;
              state[buf] = 0;
            }
          }
        }
      }
    }
      
      
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
  buffer[0][inc_pointer('w', 0, 1)] = UCA0RXBUF;
  if(write_pointer[0]==read_pointer[0])
    {write_pointer[0]--;}
}


#pragma vector=USCIAB1RX_VECTOR
__interrupt void USCI1RX_ISR(void)
{
  while (!(UC1IFG&UCA1TXIFG));
  UCA1TXBUF = 'U';
  buffer[1][inc_pointer('w', 1, 1)] = UCA1RXBUF;
  if(write_pointer[1]==read_pointer[1])
    {write_pointer[1]--;}
}
