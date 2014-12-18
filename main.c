#include "lcd.h"
#define PERIOD 5000000/4
#define PER_COUNTS ((PERIOD / 100) - 1)  // don't edit this!
#define PER_REMAINDER (PERIOD - (PER_COUNTS * 100))  // don't edit this!
unsigned long pcount=0;    // used in interrupt to count PER_COUNTS
unsigned char calc_bit=0;
unsigned char RPM_Value=0;
int C;


struct sAve
{
    unsigned long blockAverage;
    unsigned char numSamples;
};

void AddSampleToAverage(struct sAve *ave, unsigned char newSample)
{
    ave->blockAverage += newSample;
    ave->numSamples++;
}

float average; 
unsigned char GetAverage(struct sAve *ave)
{
    average =(ave->blockAverage*1.0)/(ave->numSamples);
    ave->blockAverage = 0;
    ave->numSamples = 0;
    return  (unsigned char)average;

}



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






void main(void)
{

    struct sAve myavg;

    myavg.blockAverage=0;
    myavg.numSamples=0;

TRISB=0X00;


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


            AddSampleToAverage(&myavg,RPM_Value);

            if(myavg.numSamples == 4)
            {

                C= GetAverage(&myavg);
				C=C*60*4;
               LCD_goto(2,5);
                LCD_num (C);
 

            }



            TMR1L = 0;
            TMR1H = 0;
            calc_bit=0;
            TMR1ON = 1;

        }

    }

}
















