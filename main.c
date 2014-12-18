#include "lcd.h"
unsigned char RPM_Value=0,tt;
unsigned int pcount=0;
float C;

struct sAve
{
    unsigned int blockAverage;
    unsigned char numSamples;
};

void AddSampleToAverage(struct sAve *ave, unsigned char newSample)
{
    ave->blockAverage += newSample;
    ave->numSamples++;
}

float average;
float GetAverage(struct sAve *ave)
{
    average =(ave->blockAverage*1.0)/(ave->numSamples);
    ave->blockAverage = 0;
    ave->numSamples = 0;
    return  average;

}




void delay_1s()
{

    while(pcount>0)
    {

        if (pcount % 8 ==0)
            TMR0 -=79;
        else
            TMR0 -=78;
        while(!TMR0IF) {};
        TMR0IF=0;   //the flag shoud be reset before leaving
        pcount--;
    }

    pcount = 1000;

}

void main(void)
{
    TRISB=0X00;
    initLCD();
    ADCON1 = 0x06;  // Disable Analog functions
    T1CON = 0b00001110; // TMR1 as 16-bit counter


    T0CS=0;	//internal instruction clock cycle
    PSA=0;  //selecting tmro prescaler
    PS2=1;	//prescaler defining 1:32
    PS1=0;
    PS0=0;
    TMR0IF=0;	//disabling tmr0 interrupt
	TMR0IE=0;
TMR0 -=78;


    LCD_goto(1,0);
    __delay_ms(1);
    lcd_puts ("  Tachometer  ");
    __delay_ms(1);
    LCD_goto(2,10);
    __delay_ms(1);
    lcd_puts ("RPS");
    __delay_ms(1);


    struct sAve myavg;

    myavg.blockAverage=0;
    myavg.numSamples=0;



    while(1)
    {
        TMR1L = 0;
        TMR1H = 0;
        TMR1ON = 1;
        delay_1s();
        TMR1ON = 0;

        RPM_Value = (256*TMR1H + TMR1L);

        AddSampleToAverage(&myavg,RPM_Value);

        if(myavg.numSamples == 2)
        {

            C= GetAverage(&myavg);
            C= C * 2;
			
            LCD_goto(2,1);
            LCD_num (C);
    		lcd_puts (".");
//			tt= (int)(C * 10) % 10;
			
//			LCD_num ( tt );	

        }


    }

}
