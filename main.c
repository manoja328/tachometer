#include "lcd.h"

// external crystal used 20 Mhz.

#define ONE_SECOND_X_100 500100/8  //prescalar used 1:8
#define ONE_SECOND_X_1000 5001000/8
#define TRUE 1
#define FALSE 0

unsigned char readyToCalculate = FALSE;
unsigned char captureEvent = FALSE;
unsigned char timer2flag = FALSE;
unsigned char zeroFrequency = TRUE;
unsigned int timerPast;
volatile unsigned char Roll_over_cnt=0;
volatile unsigned char rollovers_for_calc=0;

union join
{
    unsigned char parts[2];
    unsigned int whole;
};

union join timerCurrent;
//unsigned long remaining_T=0;

unsigned int calculation(unsigned int time, unsigned int * pastTime)
{
    unsigned int returnData;
    unsigned long timeBetween = (time - *pastTime)+(65535*(rollovers_for_calc-1));
    *pastTime = time;
    returnData = ONE_SECOND_X_1000 / timeBetween;

	//remaining_T = (ONE_SECOND_X_1000 - returnData * timeBetween)* 16 ;

	//remaining_T =remaining_T / 100000;

    return returnData;
}


void timers(void)
{

//    // timer 0 1ms
//    T0CS = 0; // FORC/4
//    PS2=0;
//    PS1=1;
//    PS0=1; // pre scalar 16
//    PSA = 0; // pre scalar enabled
//    TMR0 = (255 - 63); //preload timer register
//    TMR0IE = 1; //enable interrupts
//    TMR0IF = 0; // clear interrupt flag


    //timer 2 500mS timer
    TMR2ON = 0; //timer off
    T2CKPS1=T2CKPS0 = 1; // pre scalar 64
    TOUTPS3 =TOUTPS2 =TOUTPS1=TOUTPS0 = 0b1111; //post scalar 16
    PR2 = 255; //255 tmr preiod counter
    TMR2IE = 1; // enable timer interrupts
    TMR2IF = 0; // clear timer interrupt flag
    TMR2ON = 1; //timer on



    //timer 1 for input capture
    TMR1ON = 0; //timer off
    TMR1CS = 0; //forc / 4
    T1CKPS0=1;// pre scalar 8
    T1CKPS1=1;
	TMR1IE = 1; // enable timer interrupts
    TMR1IF = 0; // clear timer interrupt flag
    TMR1ON = 1; //timer on

}

void input_capure(void)
{
    TRISC2=1; //CAPTURE INPUT //RC2 input
    CCP1IF = 0; // cleanr flag
    CCP1CON = 0x05; //ccp capures on every rising edge
    //CCP1CON = 0x06; //capture edge every 4th rising edge
    //CCP1CON = 0x07; //capture edge every 16th rising edge
    CCP1IE = 1; //ccp module interrupt enable
}


void initialize(void)
{
    timers();
    input_capure();
    PEIE = 1; //enable perifiral interrupts
    GIE = 1; //enable global interrupts
}



void interrupt isr(void)
{

//    if (TMR0IF)
//    {
//        TMR0 -= (63 - 19);
//        TMR0IF = 0; //clear flag
//    }


    // timer 2 interrupt
    if (TMR2IF)
    {
        static int timeCount = 0;
        if (timeCount >= 32)
        {
            timeCount = 0;
            timer2flag = TRUE;
        } else
        {
            timeCount++;
        }
        TMR2IF = 0; //clear interrupt flag
    }


    // timer 1 interrupt
    if (TMR1IF)
    {
		Roll_over_cnt++;
        TMR1IF = 0; //clear interrupt flag
    }


    if (CCP1IF)
    {
        timerCurrent.parts[0] = CCPR1L;
        timerCurrent.parts[1] = CCPR1H;
        if (zeroFrequency == TRUE)
        {
            zeroFrequency = FALSE;
			captureEvent = TRUE;
            timerPast = timerCurrent.whole;
        }
        else
        {
            readyToCalculate = TRUE;
			captureEvent = TRUE;
        }

	
		if (Roll_over_cnt ==0)
{

rollovers_for_calc =1;
}
	
else {
rollovers_for_calc =Roll_over_cnt;
Roll_over_cnt=0;
}


        CCP1IF = 0; // cleanr flag
    }

}

main()
{
    TRISB=0X00;
    initLCD();
    ADCON1 = 0x06;  // Disable Analog functions

    LCD_goto(1,0);
    __delay_ms(1);
    lcd_puts ("  Tachometer  ");
    __delay_ms(1);


	unsigned int dataHolder=0;
    initialize();

    while(1)
    {
        if (readyToCalculate == TRUE)
        {
            dataHolder = calculation(timerCurrent.whole, &timerPast);
			readyToCalculate = FALSE;
        }


        if (timer2flag == TRUE) 
        {
                if (captureEvent == FALSE)
                {
					LCD_goto(2,0);
    				LCD_num(0);
					LCD_goto(2,5);
    				LCD_num2(0);
                } else
                {
					LCD_goto(2,0);
    				LCD_num(dataHolder);
					//LCD_goto(2,4);
					//LCD_Write('.',1);
					//LCD_goto(2,5);
    				//LCD_num2((unsigned int)remaining_T);
					//LCD_num2(Roll_over_cnt);
                    captureEvent = FALSE;
                }            
           
            timer2flag = FALSE;

        }


    }
}
