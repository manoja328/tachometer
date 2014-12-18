#include "lcd.h"


#define TRUE 1
#define FALSE 0

//multitasking system – handle multiple tasks with one microprocessor

//task counters used to tell when a task is ready to be executed
//all these counters are incremented every time a 500us interrupt happens
//every task has its own counter that is updated every time a 500us interrupt happens
unsigned int task0_counter=0;
unsigned int task1_counter=0;
unsigned int task2_counter=0;

//this tells when a task is going to happen again
//for example, when task0_counter==TASK0_COUNTER_MAX, set task0_counter=0 and do task
#define TASK0_COUNTER_MAX 1         //high frequency task – every 500us, maybe PWM
#define TASK2_COUNTER_MAX 2000      //low frequency and low priority task, every 2500ms

//Note: every variable referenced in both interrupt and main() must be declared volatile. You have been warned!
//this enables/disables a task
volatile unsigned char task0_enable=FALSE;
volatile unsigned char task2_enable=TRUE;

volatile unsigned int countmy=0;


//this allows tasks triggered by interrupt to run in the background in main()
volatile unsigned char task2_go=FALSE;
void setup_multitasking(void)
{
   //set up tmr1  to interrupt every 500us
   TMR1CS=0;
   T1CKPS0=0;
   T1CKPS1=0;

/*We want to wait 2500 clock cycles, or 500us @ 20MHz (instructions are 1/4 speed of clock).  Timer 1 interrupts when it gets to 0xFFFF or 65535.  Therefore, we set timer 1 to 65535 minus 2000 = 63535, then wait 2000 ticks until rollover at 65535.  To test, use simulator to find that its exactly correct*/

   #define  TICKS_BETWEEN_INTERRUPTS      2500
   #define  INTERRUPT_OVERHEAD            19
   #define TMR1RESET (0x0000-(TICKS_BETWEEN_INTERRUPTS-INTERRUPT_OVERHEAD))
   #define TMR1RESET_HIGH TMR1RESET >> 8
   #define TMR1RESET_LOW TMR1RESET & 0xFF
   TMR1ON=0;
   TMR1H=TMR1RESET_HIGH;
   TMR1L=TMR1RESET_LOW;
   TMR1ON=1;      
   TMR1IF=0;
   TMR1IE=1;
   PEIE=1;
   GIE=1;   
}

void interrupt isr(void)
{

	if (INTF && INTE){
	countmy++;  
    INTF = 0;          // clear the interrupt flag

}





   //one tick every 500us at 16Mhz
   if (TMR1IF && TMR1IE)
   {
      //set up timer 1 again to interrupt 500us in future
      TMR1IF=0;
      TMR1ON=0;
      TMR1H=TMR1RESET_HIGH;
      TMR1L=TMR1RESET_LOW;
      TMR1ON=1;
      task0_counter++;
      if (task0_counter>=TASK0_COUNTER_MAX)     //high frequency task – every 1 tick
      {
            task0_counter=0;
            if (task0_enable==TRUE)
            {
                  //do high frequency important task 0, for example PWM

				
            }
      }


/*this task takes a long time, 100ms for example, lots of maths.  Is extremely low priority, but has to be done at regular intervals, so all this does is trigger it.  In main(), it will, at leisure, poll ‘task2_go’ and then execute it in the background.*/

      task2_counter++;
      if (task2_counter>=TASK2_COUNTER_MAX)     //every 250ms
      {
          task2_counter=0;
          if (task2_enable==TRUE)
          {
               //every 250ms take 100ms to do maths, do this in main() so the we can get back to doing the high frequency tasks.
               task2_go=TRUE;
				INTE = 1; 
          }
      }
   }  //if (TMR1IF)
} //interrupt routine

main()
{
    TRISB=0X01;
    initLCD();
    ADCON1 = 0x06;  // Disable Analog functions

    LCD_goto(1,0);
    __delay_ms(1);
    lcd_puts ("  Tachometer  ");
    __delay_ms(1);


    INTE = 1;                //Enable RB0/INT external Interrupt
    INTEDG = 1;              //Interrupt on rising edge

   setup_multitasking();
   while(1)
   {
      if (task2_go==TRUE)
      {
            task2_go=FALSE;
            //take our time, doing heaps of complex maths at our leisure in the background

	        LCD_goto(2,0);
        	LCD_num (countmy);
			countmy=0;

		
      }
   }
}