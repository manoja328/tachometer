#include <htc.h>

__CONFIG(HS & WDTDIS & PWRTEN & BORDIS);

#define _XTAL_FREQ 20000000

#define LCD_RS RB7
#define LCD_EN RB6
#define LCD_D4 RB5
#define LCD_D5 RB4
#define LCD_D6 RB3
#define LCD_D7 RB2


void strobe()
{
    LCD_EN = 0;
    __delay_us(1);
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(100);

}




void LCD_Write(unsigned char values,int rs)
{

    LCD_RS = rs; // rs=0 command and rs=1 data

    //MSB

    LCD_D7=(values & 0x80)?(1:0);
    LCD_D6=(values & 0x40)?(1:0);
    LCD_D5=(values & 0x20)?(1:0);
    LCD_D4=(values & 0x10)?(1:0);

    strobe();

    //LSB

    LCD_D7=(values & 0x08)?(1:0);
    LCD_D6=(values & 0x04)?(1:0);
    LCD_D5=(values & 0x02)?(1:0);
    LCD_D4=(values & 0x01)?(1:0);

    strobe();



}

void LCD_clear(void)
{
    LCD_Write(0x01,0);		//this clears LCD
}

void LCD_goto(unsigned char row,unsigned char column)
{
    if(row==1)
    {
        LCD_Write(0x80+column,0);
    }
    else if(row==2)
    {
        LCD_Write(0xC0+column,0);
    }
}


void lcd_puts(const char *s)
{

    while(*s)
    {
        __delay_ms(1);
        LCD_Write(*s++,1);

    }
}

void LCD_num(int n)
{ 
    if(n<=9999)
    {
        LCD_Write((n/1000)+48,1);
        LCD_Write(((n/100)%10)+48,1);
        LCD_Write(((n%100)/10)+48,1);
        LCD_Write((n%10)+48,1);

    }


    if(n>9999)
    {
		lcd_puts("XXXX");

    }
    
    

}


void LCD_num2(int n)
{ 
    if(n<=99)
    {
        LCD_Write((n/10)+48,1);
        LCD_Write((n%10)+48,1);

    }
 

}





void lcd_putch(char c)
{
    __delay_ms(1);
    LCD_Write( c,1 );
}



void initLCD()
{



    LCD_RS = 0;
    LCD_EN = 0;

    __delay_ms(15);	// wait 15mSec after power applied,

    LCD_D4 = 1;//0x3 & 0x01;				//bit0 000X
    LCD_D5 = 1;//(0x3>>1) & 0x01;		//bit1 00XY -> 000X
    LCD_D6 = 0;//(0x3>>2) & 0x01;		//bit2 0XYZ -> 000X
    LCD_D7 = 0;//(0x3>>3) & 0x01;		//bit3 XYZW -> 000X

    strobe();
    __delay_ms(5);
    strobe();
    __delay_ms(5);
    strobe();
    __delay_us(150);

    // Four bit mode
    LCD_D4 = 0;//2 & 0x01;				//bit0 000X
    LCD_D5 = 1;//(2>>1) & 0x01;		//bit1 00XY -> 000X
    LCD_D6 = 0;//(2>>2) & 0x01;		//bit2 0XYZ -> 000X
    LCD_D7 = 0;//(2>>3) & 0x01;		//bit3 XYZW -> 000X

    strobe();

    LCD_Write(0x28,0);    // Set interface length: nibblemode, 2line, 5x7dot

    LCD_Write(0x0C,0);    // Display On, Cursor Off, Cursor Blink off
    __delay_ms(1);

    LCD_clear();
    LCD_Write(0x6,0); // Set entry Mode : increment, displayShiftOff


}
