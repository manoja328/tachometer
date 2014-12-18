#include "lcd.h"

#define PERIOD 5000000
#define PER_COUNTS ((PERIOD / 100) - 1)  // don't edit this!
#define PER_REMAINDER (PERIOD - (PER_COUNTS * 100))  // don't edit this!
unsigned long pcount=0;    // used in interrupt to count PER_COUNTS
unsigned char calc_bit=0;
unsigned long RPM_Value=0;


void interrupt isr()
{

    if(!pcount)
    {

        TMR1ON = 0;    // Stop the timer	
        calc_bit=1;
        pcount = (PER_COUNTS+1);    // how many delays to make total
        TMR0 -=(PER_REMAINDER-3);  // first delay will be ==remainder
    }
    // else make a normal delay

    else
    {


		
        TMR0 -= (100-2);       // make another 100 tick delay
    }
    pcount--;
    //-----------------------------------------------------
    // clear the TMR0 overflow flag and exit
    T0IF = 0;
}

unsigned char delay;
void delay_10us(unsigned char t)
// Provides t * 10 usecs of delay.
// Max of t is 255 which corresponds to 2550 usecs 
{
delay = t;
/*  Below is an assembly language fragment to 
 *  create a 10 microsecond delay.  Note that in 
 *  this fragment the C global variable "delay" 
 *  is written as "_delay".
 */
#asm
DELAY_10US_1: 
   CLRWDT 
   NOP
   NOP
   NOP
   NOP
   NOP
   NOP 
   DECFSZ  _delay,f 
   GOTO    DELAY_10US_1
#endasm 
} 




void main(void)
{
    TRISB = 0b00000000;


    initLCD();
    ADCON1 = 0x06;  // Disable Analog functions
    T1CON = 0b00001110; // TMR1 as 16-bit counter




   	LCD_goto(1,0);
    __delay_ms(1);
    lcd_puts ("  Tachometer  ");
    __delay_ms(1);
    LCD_goto(2,10);
    __delay_ms(1);
    lcd_puts ("RPM");
    __delay_ms(1);

	TMR0 =0;
    OPTION = 0b00001000;    // TMR0 on, 1:1 prescale
    INTCON = 0b10100000;  // GIE on, T0IE on (turn interrupt on)

    while(1)
    {
        
        if (calc_bit==1)
        {

            RPM_Value = (256*TMR1H + TMR1L) ;
            LCD_goto(2,5);
            LCD_num (RPM_Value);
            TMR1L = 0;
            TMR1H = 0;
            calc_bit=0;
            TMR1ON = 1;            

        }

    }

}





