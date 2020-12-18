


0
OUT = 2.0, 1.1, 2.2, 2.3, 2.7, 2.5
P2OUT = 0b01000000 = 64
P1OUT &= ~BIT1;

1
OUT = 2.5,2.7
P2OUT = 0b01001101 = 77

2
OUT = 2.0, 1.1, 2.6, 2.7, 2.3
P2OUT = 0b00100100 = 36
P1OUT &= ~BIT1;

3
OUT = 2.0, 1.1, 2.6, 2.2, 2.3
P2OUT = 0b10100000 = 160
P1OUT &= ~BIT1;

4
OUT = 2.5, 2.6, 1.1, 2.2
P2OUT = 0b10001001 = 141
P1OUT &= ~BIT1;

5
OUT = 2.0, 2.5, 2.6, 2.2, 2.3
P2OUT = 0b10000000 = 128

6
OUT = 2.0, 2.5, 2.7, 2.3, 2.2, 2.6
P2OUT = 0b00000000 = 0

7
OUT = 2.0, 1.1, 2.2
P2OUT = 0b11101000 = 232
P1OUT &= ~BIT1;

8
OUT = 2.0, 2.5, 1.1, 2.7, 2.2, 2.3, 2.6
P2OUT = 0b00000000 = 0
P1OUT &= ~BIT1;

9
OUT = 2.0, 2.5, 1.1, 2.2, 2.6
P2OUT = 0b10001000 = 136
P1OUT &= ~BIT1;

a
OUT = 2.0, 2.5, 1.1, 2.7, 2.2, 2.6
P2OUT = 0b00001000 = 8
P1OUT &= ~BIT1;

b
OUT = 2.5, 2.7, 2.3, 2.2, 2.6
P2OUT = 0b00000001 = 1

c
OUT = 2.0, 2.5, 2.7, 2.3
P2OUT = 0b01000100 = 68

d
OUT = 1.1, 2.7, 2.2, 2.3, 2.6
P2OUT = 0b00100001 = 33
P1OUT &= ~BIT1;

e
OUT = 2.0, 2.5, 2.7, 2.3, 2.6
P2OUT = 0b00000100 = 4

f
OUT = 2.0, 2.5, 2.7, 2.6
P2OUT = 0b00001100 = 12

r
OUT = 2.7, 2.6
P2OUT = 0b00101101 = 45

g
OUT = 2.0, 2.5, 1.1, 2.2, 2.3, 2.6
P2OUT = 0b10000000 = 128
P1OUT &= ~BIT1




#include <msp430.h>

int powerModeCounter = 0;

int main (void){                //RED = 1.4 | GREEN = 1.6 | BLUE = 2.1       //125000 cycles every second
    
    WDTCTL = WDTHOLD | WDTPW;       //hold washdogtimer so it doesnt reset
    BCSCTL1 = CALBC1_1MHZ;          //calibrate CPU to 1 megahertz
    DCOCTL = CALDCO_1MHZ;
    
    P2DIR = BIT1;                   //blue led 2.1
    P1OUT = 0b00001000;             //*set button as output
    P2SEL = BIT1;                   //blue led 2.1
    P1SEL = BIT6;                   //green led 1.6
    
    P1REN = BIT3;                   //button
    P1IE |= BIT3;
    P1IFG &= ~BIT3;
    P1IES |= BIT3;
    _BIS_SR(GIE);
    
    for(;;){
        
        //powerMode 1
        if(powerModeCounter == 1){
            
            P1OUT = 0b00011000;
            P1DIR = BIT4;
            P1OUT |= BIT4;
            P2DIR = 0b00000000;
            
            P1OUT = 0b00011000;                    //red led blinks for (1/2) second (execution loop)
            __delay_cycles(500000);
            P1OUT = 0b00001000;
            __delay_cycles(500000);
        }
        
        //powerMode 2                              //red led (1/2 second) + green led(1/3 second)
        else if(powerModeCounter == 2){
            
            P1SEL = BIT6;
            P1DIR = 0b01010000;                    //red led and green led8
            P1OUT |= 0b01011000;
            
            TA0CTL   = TASSEL_2 | MC_1 | ID_3;     //green led (1/3) second (change to interrupt and timer)
            TA0CCR0  = 41666 - 1;                  //calibrate to every 1/3 second)
            TA0CCTL1 = OUTMOD_4;
            
            P1OUT = 0b00011000;                    //red led (1/2) second (execution loop)
            __delay_cycles(500000);
            P1OUT = 0b00001000;
            __delay_cycles(500000);
        }
        
        //powerMode 3                              //blue led(1/4 second) + green led(1/3 second)
        else if(powerModeCounter == 3){
            
            P1DIR = 0b01000000;
            P2DIR = 0b00000010;
            P1OUT |= 0b00001000;
            P2OUT = 0b00000010;
            
            TA1CTL   = TASSEL_2 | MC_1 | ID_3;     //timer and signal (blue led)
            TA1CCR0  = 31250 - 1;                  //calibrate to every 1/4 second
            TA1CCTL1 = OUTMOD_4;
            
            TA0CTL   = TASSEL_2 | MC_1 | ID_3;     //green led (1/3) second (change to interrupt and timer)
            TA0CCR0  = 41666 - 1;                  //calibrate to every 1/3 second (timer and signal)
            TA0CCTL1 = OUTMOD_4;
        }
        
        //powerMode 4                              //blue led(1/2 second)
        else if(powerModeCounter == 4){
            
            P1OUT = 0b00001000;
            P1DIR &= ~BIT6;
            P2DIR = 0b00000010;
            P2OUT = 0b00000010;
            
            TA1CTL   = TASSEL_2 | MC_1 | ID_3;     //timer and signal (blue led)
            TA1CCR0  = 62500 - 1;                  //calibrate to every 1/2 second
            TA1CCTL1 = OUTMOD_4;
        }
    }                                          //END OF LOOP
    return 0;
}                                          //END OF MAIN


//button_ISR
#pragma vector=PORT1_VECTOR
__interrupt void buttonInterrupt(void){
    
    powerModeCounter++;
    
    if(powerModeCounter > 4){
        powerModeCounter = 1;
    }
    
    while(!(BIT3 & P1IN))  {}
    __delay_cycles(32000);
    P1IFG &= ~BIT3;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void){
    
    //Instantiate desired array to be printed                         //TEST HEREEEEE************
    desiredDisplay[0] = firstValue / 16;
    desiredDisplay[1] = firstValue % 16;
    desiredDisplay[2] = secondValue / 16;
    desiredDisplay[3] = secondValue % 16;
    
    P1OUT = 0b00001111;                                             // turn off all digits
    digitCounter++;                                                 // figure out next digit
    // display digit
    if(digitCounter < 1 || digitCounter > 4)      // handle case of digitCounter at 0 or 5
        digitCounter = 1;
    if(firstValue == 0 && secondValue == 0)       // ensures all digits are off unless user inputs data
        P1OUT = 0;
    else
        P1OUT = digitCounterArray[digitCounter-1];          // Display desiredDigit
    
    P2DIR = displays[desiredDisplay[digitCounter-1]];       // Display 1 desiredValue
    //leave interrupt
}






