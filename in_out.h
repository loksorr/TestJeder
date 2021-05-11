#ifndef __INOUT_H__
#define __INOUT_H__

//------------------------------------------------------
#define KEY_START                       BIT0 // P1.0 vhod
//------------------------------------------------------
#define AMP_GAIN_ENABLE                 BIT1 // Pin P1.1 izhod
#define AMP_GAIN_SELECT_0               BIT2 // Pin P1.2 izhod
#define AMP_GAIN_SELECT_1               BIT3 // Pin P1.3 izhod

#define CORE1_LED_GREEN                 BIT4 // Pin P1.4 izhod
#define CORE1_LED_RED                   BIT5 // Pin P1.5 izhod
#define CORE2_LED_GREEN                 BIT6 // Pin P1.6 izhod
#define CORE2_LED_RED                   BIT7 // Pin P1.7 izhod

//------------------------------------------------------
#define KEY_SELECT_CORE                 BIT0 // P2.0 vhod
//------------------------------------------------------

//------------------------------------------------------
// Nastavitve za zunanji PGA ( programabilni gain amp. )
//------------------------------------------------------
#define AMP_GAIN_x1           0
#define AMP_GAIN_x2           AMP_GAIN_SELECT_0        
#define AMP_GAIN_31mA         AMP_GAIN_SELECT_1       
#define AMP_GAIN_65mA         AMP_GAIN_SELECT_0 + AMP_GAIN_SELECT_1      
//------------------------------------------------------
//----------------------------
// TASKS (opravila)
//----------------------------
#define TASK_INIT             0x0001
#define TASK_POWER_ON         0x0002

#define TASK_50Hz             0x0010
#define TASK_DC               0x0040

#define TASK_END              0x0800
#define TASK_SUCCESS          0x1000
#define TASK_FAIL             0x2000

//----------------------------
// TASKS STATUS
//----------------------------
#define STATUS_DEFAULT               0x0000
#define STATUS_OK                    0x0001
#define STATUS_POWER_OK              0x0002
#define STATUS_TIMEOUT               0x0004

#define STATUS_ADC_OK                0x0010

#define STATUS_ERROR                 0x0100
#define STATUS_ERROR_POWER_CURRENT   0x0200
#define STATUS_ERROR_POWER_VOLTAGE   0x0400
#define STATUS_ERROR_AC_CURRENT      0x0800
#define STATUS_ERROR_DC_CURRENT      0x1000
#define STATUS_ERROR_LED             0x2000


//------------------------------------------------------
//DAC12_0                     BIT6 // P6.6 izhod
//DAC12_1                     BIT7 // P6.7 izhod
//------------------------------------------------------

//------------------------------------------------------
// Dolzina generiranega signala ( cosinus 50, 1000 Hz)
//------------------------------------------------------

#define SIGNAL_LENGTH 200

//------------------------------------------------------

#endif /*__INOUT_H__*/