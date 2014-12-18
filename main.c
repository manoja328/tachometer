#include "lcd.h"

void main(void)
{
    unsigned long RPM_Value;
    unsigned int i=0;

    TRISB = 0b00000001;
    
    initLCD();
    ADCON1 = 0x06;  // Disable Analog functions
    T1CON = 0b00001110; // TMR0 as 16-bit counter

    LCD_goto(1,0);
    __delay_ms(1);
    lcd_puts ("   TACHOMETER   ");
    __delay_ms(1);


    LCD_goto(2,0);
    __delay_ms(1);
    lcd_puts ("RPM:");
    __delay_ms(1);

    while(1)
    {

        
        TMR1L = 0;
        TMR1H = 0;
        TMR1ON = 1;



        for(i=0; i<100; i++)
            __delay_ms(10);       
            
            
        TMR1ON = 0;    // Stop the timer
        
        RPM_Value = (256*TMR1H + TMR1L);
        LCD_goto(2,5);
        LCD_num (RPM_Value);

    }

}





