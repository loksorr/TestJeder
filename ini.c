#include <msp430.h>
#include "ini.h"
#include "lcd_driver.h"
#include "cos_200.h"
#include "in_out.h"

//------------------------------------------------------------------------------
// spremenljivke casa
//------------------------------------------------------------------------------
unsigned int time_1ms;
unsigned int time_10ms;
unsigned int time_seconds;

//=====================================
unsigned int time_200ms_flag;
//=====================================



//==============================================================================
// AC SIGNAL
//==============================================================================
//------------------------------------------------------------------------------
// Amplifier 50 Hz Gain ( 1x, 2x, 5x )
//------------------------------------------------------------------------------
unsigned int amp_gain_50Hz[3] = { AMP_GAIN_x1 , AMP_GAIN_31mA, AMP_GAIN_65mA };

unsigned int time;
unsigned int time_start;

unsigned int task_status;

unsigned int return_result;

//------------------------------------------------------------------------------

unsigned int opravilo;
unsigned int opravilo_finished;

//------------------------------------------------------------------------------


void init_pins(void)
{
  //-------------------------------------------------------------------
  P1DIR = (unsigned char)( ~KEY_START ); // All P1.x outputs except...
  P1SEL = 0;    // All P1.x GPIO function
  P1OUT = 0;
  
  P1REN = KEY_START; // pullup/pulldown resistor enabled
  P1OUT = KEY_START; // the pin is pulled up 

  //-------------------------------------------------------------------
  // Vse pine moras inicializirat, definirat stanje, drugace 
  // je obnasanje nedefinirano (releji sklocajo)
  //-------------------------------------------------------------------

  P2DIR = (unsigned char)( ~KEY_SELECT_CORE ); // All P2.x outputs except...
  P2SEL = 0;
  P2OUT = 0;

  //-------------------------------------------------------------------
  // LCD (P3)
  //-------------------------------------------------------------------
  P3DIR = DB4 + DB5 + DB6 + DB7 + E + RW + RS;
  P3OUT = 0x00; 
  //-------------------------------------------------------------------
  
  //-----------------------------------------
  // enable P6.6-7 for DAC0 & DAC1
  //-----------------------------------------
  P6DIR = BIT6 + BIT7;
  
  P6SEL |= ( BIT6 + BIT7 ); // enable P6.6-7 for DAC0 & DAC1
  
  P6OUT = 0x00;
  //-----------------------------------------
}

void Set_Clk(void)
{
  //-------------------------------------------------------------------
  DCOCTL = 0;            // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ; // Set range
  DCOCTL = CALDCO_1MHZ;  // Set DCO step + modulation */
  
  //-------------------------------------------------------------------
  // 50 ms delay
  //-------------------------------------------------------------------
  __delay_cycles( 50000 );
  
  //-------------------------------------------------------------------
  BCSCTL1 = CALBC1_16MHZ | 0x80; // Set DCO
  DCOCTL = CALDCO_16MHZ;
  //-------------------------------------------------------------------
  
  __delay_cycles( 10000 ); // Wait for DCO to stabilize.
  
  FCTL2 = FWKEY + FSSEL0 + 0x10; 
}


void DAC_init(void)
{
  DAC12_0CTL &= ~DAC12ENC; 

  DAC12_0CTL =               // DAC12_0 output on P6.6 , DAC12_1 output on P6.7 !
              + DAC12SREF_2  // DAC12_0 reference 2: Veref+ (pin 10 on MSP430)
              + DAC12RES_0   // 12-bit resolution
              + DAC12LSEL_0  // DAC12 load select 0: direct 
              + DAC12CALON // Calibration IS IN PROGRESS   
              + DAC12IR      // DAC12 full-scale output = 1x reference voltage 
              + DAC12AMP_5   // Medium speed/current
                             //+ DAC12AMP_1   // DAC12 off, output 0V
              + DAC12DF_0;    // Straight binary     
  
  DAC12_0CTL |= DAC12ENC;     // DAC12 enable conversion
  
  while( DAC12_0CTL & DAC12CALON ); // wait for calibration to complete
  
  DAC12_1CTL &= ~DAC12ENC;
    
  DAC12_1CTL =                  // DAC12_0 output on P6.6 , DAC12_1 output on P6.7 !
                 + DAC12SREF_2   // DAC12_1 reference 2: Veref+ (pin 10 on MSP430)
                 + DAC12RES_0    // 12-bit resolution    
                 + DAC12LSEL_0   // DAC12 load select 0: direct 
                 + DAC12CALON // Calibration IS IN PROGRESS    
                 + DAC12IR       // DAC12 full-scale output = 1x reference voltage
                 + DAC12AMP_5    // DAC12 amplifier 5: medium, medium
                 //+ DAC12AMP_1    // DAC12 off, output 0V
                 + DAC12DF_0     // Straight binary     
                 + DAC12ENC;     // DAC12 enable conversion
  
  DAC12_1CTL |= DAC12ENC;     // DAC12 enable conversion

  while( DAC12_1CTL & DAC12CALON ); // wait for calibration to complete
  
  DAC12_1DAT = 0x000;
  DAC12_0DAT = 0x000;
}



void DAC_init_AC(void)
{
  // DMA settings
  //--------------------------------------------
  __data16_write_addr((unsigned short) &DMA2SA,(unsigned long) Signal);      // Source block address
  __data16_write_addr((unsigned short) &DMA2DA,(unsigned long) &DAC12_1DAT); // Destination single address
  
  //----------------------------------------------------------------------------
  // Timer_B operates in upmode with TBCCR2 loading DAC12_1 on the rising edge, CCR2_CCIFG
  // triggering the next DMA2 transfer.
  //----------------------------------------------------------------------------

  DMA2SZ = SIGNAL_LENGTH; // Block size
  
  DMACTL0 = DMA2TSEL_2;  // TBCCR2 trigger
   
  
  DMA2CTL =   DMASWDW       // source word to destination word 
            + DMADT_4       //Repeated single transfer
            + DMASRCINCR_3 // DMA source increment 3: source address incremented
            + DMAEN;       // DMA enable
  //----------------------------------------------------------------------------
  DAC12_0CTL &= ~DAC12ENC;
  DAC12_0CTL &= ~DAC12LSEL_3; // DAC12 latch loads when written data
  DAC12_0CTL |= DAC12ENC;
  
  DAC12_1CTL &= ~DAC12ENC;
  DAC12_1CTL |= DAC12LSEL_3; //generate sinus: load Timer_B2
  DAC12_1CTL |= DAC12ENC;
  //----------------------------------------------------------------------------
}

void DAC_init_DC(void)
{
  DAC12_0CTL &= ~DAC12ENC;
  DAC12_0CTL &= ~( DAC12LSEL_3 + DAC12IFG + DAC12IE ); // DAC12 load select 0: direct data 
  DAC12_0CTL |= DAC12ENC;
  
  DAC12_1CTL &= ~DAC12ENC;
  DAC12_1CTL &= ~( DAC12LSEL_3 + DAC12IFG + DAC12IE ); // DAC12 load select 0: direct data 
  DAC12_1CTL |= DAC12ENC;
    
  DAC12_1DAT = 0x000;
  DAC12_0DAT = 0x000;
}

void DAC_Stop(void)
{
  //-----------------------------------------------------
  DMACTL1 |= ENNMI; // stop DMA
  
  DMA2CTL &= ~DMAEN;
  
  TBCCR0 = 0;
  TBCCR2 = 0;

  TBCTL = TBCLR; // ustavi TimerB
  
  TBCCTL0 &= ~CCIE; // disable TB0
  
  TBCCTL2 &= ~CCIE; // disable TB2
  
  TBCCTL2 &= ~( CCIFG | COV ); // clear flags
  
  TBCCTL0 &= ~( CCIFG | COV ); // clear flags

  DMA2CTL &= ~( DMAIFG + DMAIE + DMAABORT );
  
  DMACTL1 &= ~ENNMI;
  //-----------------------------------------------------
  
  DAC_init_DC();
}

//-----------------------------------------------------------------------------

void DAC_run_50( void )
{
  TBCCTL2 = OUTMOD_7;// Reset/set
  
  TBCCR2 = 1;       // PWM Duty Cycle
  //----------------------------------------------------------------------------
  // DMA se prozi na 100 us, 100us x 200 tock = 20 ms -> 50 Hz
  //----------------------------------------------------------------------------
  TBCCR0 = 100-1; // Clock period of TBCCR0
  //---------------------------------------------------------------------------- 
  TBCTL = TBSSEL_2 + ID_3 +  MC_1; // SMCLK / 8 = 1 MHz, up mode
}

//-----------------------------------------------------------------------------




