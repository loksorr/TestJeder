#include <msp430.h>
#include "in_out.h"
#include "ini.h"
#include "timer.h"

//--------------------------------------------------------------------------

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1( void )
{
}
//--------------------------------------------------------------------------

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2( void )
{ 

}
//--------------------------------------------------------------------------

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
#else
#error Compiler not supported!
#endif
{
  
}
