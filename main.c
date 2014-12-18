#include "lcd.h"
unsigned char RPM_Value=0;
unsigned int pcount=0;    
void delay_1s()
{

	while(pcount>0)
	{

		if (pcount % 4 ==0)
			TMR0 -=157;
		else
			TMR0 -=156;
		while(!TMR0IF) {};
		TMR0IF=0;   //the flag shoud be reset before leaving                                
		pcount--;
	}

	pcount = 2000;
   
}

void main(void)
{
    TRISB=0X00;
    initLCD();
    ADCON1 = 0x06;  // Disable Analog functions
    T1CON = 0b00001110; // TMR1 as 16-bit counter
    TMR1L = 0;
    TMR1H = 0;


    T0CS=0;	//internal instruction clock cycle
    PSA=0;  //selecting tmro prescaler
    PS2=1;	//prescaler defining 1:128
    PS1=0;
    PS0=0;
    TMR0IF=0;	//disabling tmr0 interrupt


    LCD_goto(1,0);
    __delay_ms(1);
    lcd_puts ("  Tachometer  ");
    __delay_ms(1);
    LCD_goto(2,10);
    __delay_ms(1);
    lcd_puts ("RPS");
    __delay_ms(1);


    while(1)
    {
        

        TMR1ON = 1;
        delay_1s();
        TMR1ON = 0;

        RPM_Value = (256*TMR1H + TMR1L)  ;
        TMR1L = 0;
        TMR1H = 0;
        LCD_goto(2,5);
        LCD_num (RPM_Value/2);

    }

}
