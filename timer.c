#include <msp430.h>
#include "ini.h"
#include "timer.h"
#include "in_out.h"

void init_timer(void)
{
    TACTL = TACLR; // Timer_A clear

    TACCTL0 = CM_0; // No capture
    
    TACCTL1 = CM_0;
    
    TACCR2 = 10000; // 10 ms
        
    TACCTL2 = OUTMOD_4 + CCIE; // Output is toggled when the timer counts to the TACCRx value
    
    TACTL = TASSEL_2 + MC_2 + ID_1 + TAIE; // ACLK, cont. mode, divide by 8 => 1 MHz, timer owerflow interrupt enabled,    
}

void TimerA0_start( void )
{
  TACCTL0 &= ~CCIE; // disable TA0
  
  TACCTL0 &= ~( CCIFG | COV ); // clear flags
  
  TACCR0 = TAR + 1000; // 1 ms
  
  TACCTL0 |= CCIE;
  
  time_1ms = 0;
}


#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
 TACCR0 += 1000; 
  
 time_1ms++; // increase miliseconds
}

// Timer_A3 Interrupt Vector (TAIV) handler
#pragma vector=TIMERA1_VECTOR
__interrupt void Timer_A1(void)
{
  switch( TAIV )
  {
  case  2: // TACCR1
    { 
      break;
    }
  case  4:
    {  
      TACCR2 = 9900; // 10ms  Add Offset to CCR2
      
      //------------------------------------------------
      // Time ( 10 ms, 200ms, 1 second )
      //------------------------------------------------
      time_10ms++;
      
      if( ( time_10ms % 20 ) == 0 )
      {
        time_200ms_flag = 1;
      }
      if( time_10ms >= 100 )
      {
        time_10ms = 0;
        time_seconds++;  
      }
      break;
    } 
  case 10:
    {    
      break;
    }
  }
}