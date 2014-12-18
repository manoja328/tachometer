#include "lcd.h"
unsigned char RPM_Value=0;

void delay_Nms(int n)
{
    while(n>0)
    {
        TMR0=217;
        while(!TMR0IF) {}
        TMR0IF=0;   //the flag shoud be reset before leaving ISR
        n--;
    }
}

void main(void)
{
    TRISB=0X00;

    T0CS=0;	//internal instruction clock cycle
    PSA=0;  //selecting tmro prescaler
    PS2=1;	//prescaler defining 1:128
    PS1=1;
    PS0=0;
    TMR0IF=0;	//disabling tmr0 interrupt


    initLCD();
    ADCON1 = 0x06;  // Disable Analog functions
    T1CON = 0b00001110; // TMR1 as 16-bit counter
    TMR1L = 0;
    TMR1H = 0;

    LCD_goto(1,0);
    __delay_ms(1);
    lcd_puts ("  Tachometer  ");
    __delay_ms(1);
    LCD_goto(2,10);
    __delay_ms(1);
    lcd_puts ("RPM");
    __delay_ms(1);


    while(1)
    {
        TMR1ON = 1;
        delay_Nms(1000);
        TMR1ON = 0;
        RPM_Value = (256*TMR1H + TMR1L) ;
        TMR1L = 0;
        TMR1H = 0;
        LCD_goto(2,5);
        LCD_num (RPM_Value);

    }

}
