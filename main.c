#include "lcd.h"
unsigned char count=0,count2=0,calc_bit=0,mycalc;

void interrupt isr()
{

	if (INTF==1){   
	TMR1ON=0;      
	calc_bit=1;
    INTF = 0;   
}


		if(TMR1IF==1)  {	
		count++; 		
		TMR1IF=0; 


}


}


void init_EXT0()
{
    GIE = 1;                  //Enable Global Interrupt
    INTE = 1;                //Enable RB0/INT external Interrupt
    PEIE = 1;                //Disable all unmasked peripheral interrupt
    INTEDG = 1;              //Interrupt on rising edge

}

void init_TMR1()
{
		TMR1H=0x00;
		TMR1L=0x00;
	
		T1CKPS1=0;
		T1CKPS0=0;
	
		TMR1CS=0;
		TMR1IF=0;
		TMR1IE=1;
		TMR1ON=1;

}

void main(void)
{
    TRISB=0X01;
    initLCD();
    ADCON1 = 0x06;  // Disable Analog functions

    LCD_goto(1,0);
    __delay_ms(1);
    lcd_puts ("  Tachometer  ");
    __delay_ms(1);

//    LCD_goto(2,10);
 //   __delay_ms(1);
 //   lcd_puts ("RPS");
 //   __delay_ms(1);


	init_TMR1();

init_EXT0();


    while(1)
    {
        PORTC=0;

if (calc_bit){
		calc_bit=0;
        LCD_goto(2,0);
        LCD_num (count);
        LCD_goto(2,5);
        LCD_num (TMR1H);
        LCD_goto(2,10);
        LCD_num (TMR1L);
		count=0;
		TMR1H=0x00;
		TMR1L=0x00;
		TMR1ON=1; 



}
    }
}



