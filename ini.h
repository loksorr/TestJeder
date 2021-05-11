#ifndef __INI_H__
#define __INI_H__

#define DAC12RES_0      0
#define DAC12CALON_0    0
#define DAC12DF_0       0



#define FALSE    0
#define TRUE    -1
//-----------------------------------------
extern unsigned int time_1ms;

extern unsigned int time_10ms;
extern unsigned int time_seconds;

extern unsigned int time_200ms_flag;

extern unsigned int amp_gain_50Hz[];

extern unsigned int time;
extern unsigned int time_start;

extern unsigned int task_status;

extern unsigned int return_result;

//-----------------------------------------

extern unsigned int opravilo;
extern unsigned int opravilo_finished;

void init_pins(void);
void Set_Clk(void);

void DAC_init(void);

void DAC_Stop(void);
void DAC_run_50(void);
void DAC_init_AC(void);


#endif