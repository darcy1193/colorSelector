//
//  colors.c
//
//  - 2.1 = GREEN LED | 2.4 = BLUE LED | 1.2 = RED LED
//  - TA0CCRO = 7 DIGIT DISPLAY
//  - 1.5,1.6,1.7 = DIGITS
//
//  Created by Daniel Darcy on 3/29/17.
//  Assighment 5 CS 2230

#include <msp430.h>

//CONSTANTS     *******************************************

// Each value in array displays corresponds to hex value [0,15] as well as r,g
int displays[18] = {64,77,36,160,137,128,0,232,0,136,8,1,68,33,4,12,45,128};
int displaysBIT1[18] = {1,0,1,1,1,0,0,1,1,1,1,0,0,1,0,0,0,1};

// Each value in array corresponds to specific digit to display [1,3]
int digitCounterArray[3] = {32,64,128};

//GLOBAL VARIABLES      ******************************************
int powerModeCounter = 1;             //LED
int digitCounter = 1;                 //DISPLAY
int desiredDisplay[3];                //ARRAY OF CHARACTERS TO DISPLAY
unsigned int number = 0;

int main (void){                      //MAINNNNNN       ***********************
    
    WDTCTL = WDTHOLD | WDTPW;         //WIZARD WORDS
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    TA0CTL   = TASSEL_2 | MC_1 | ID_3;      //INSTANTIATE DISPLAY TIMER
    TA0CCTL0 = CCIE;
    TA0CCR0  = 500 - 1;               //1000 you can see the drag, 500 seems crispest
    TA0CCTL1 = OUTMOD_4;              //CHANGED FROM OUTMOD_4
    
    P1OUT = BIT3;                     //BUTTON
    P1REN = BIT3;
    P1IE |= BIT3;
    P1IFG &= ~BIT3;
    P1IES |= BIT3;
    _BIS_SR(GIE);                     //ENABLE GLOBAL INTERRUPTS
    
    P1DIR = -1;                       //7-DIGIT DISPLAY ETC.
    P2SEL &= ~(BIT6 | BIT7);
    P2DIR = -1;
    P2OUT = 0;
    
    P1DIR &= ~BIT3;                   //SET AS INPUTS
    P1DIR &= ~BIT4;
    
    initialize_dtc(INCH_4, &number);  //STORES VALUE 0 - 1023 in number
    for(;;){}                         //INFINITE LOOP

    return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void buttonInterrupt(void){           //buttonInterrupt     ****************
    
    powerModeCounter++;
    
    if((powerModeCounter > 3) | (powerModeCounter < 1)){
        powerModeCounter = 1;
    }
    while(!(BIT3 & P1IN))  {}                     //KEEPS BUTTON CONSISTANT
    __delay_cycles(32000);
    
    P1IFG &= ~BIT3;
}

#pragma vector=TIMER0_A0_VECTOR                   //Timer0_A0_ISR       ***************
__interrupt void Timer0_A0_ISR (void){
    
    //DECIDE WHAT COLOR LED TO DISPLAY
    if(powerModeCounter == 1){       //RED
        P1DIR |= BIT2;
        P1SEL |= BIT2;
        TA0CTL   = TASSEL_2 | MC_1 | ID_3;            //INSTANTIATE TIMER
        TA0CCR0 = 1023;
        TA0CCR1 = number;
        TA0CCTL1 = OUTMOD_7;
        desiredDisplay[0] = 16;                       //SET FIRST CHAR
    }
    else if(powerModeCounter == 2){  //GREEN
        P2DIR |= BIT1;
        P2SEL |= BIT1;
        TA1CTL   = TASSEL_2 | MC_1 | ID_3;            //INSTANTIATE TIMER
        TA1CCR0 = 1023;
        TA1CCR1 = number;
        TA1CCTL1 = OUTMOD_7;
        desiredDisplay[0] = 17;                       //SET FIRST CHAR
    }
    else if(powerModeCounter == 3){  //BLUE
        P2DIR |= BIT4;
        P2SEL |= BIT4;
        TA1CTL   = TASSEL_2 | MC_1 | ID_3;            //INSTANTIATE TIMER
        TA1CCR0 = 1023;
        TA1CCR2 = number;
        TA1CCTL2 = OUTMOD_7;
        desiredDisplay[0] = 11;                       //SET FIRST CHAR
    }
    
    number = number >> 2;                         //(10 bit) - (8 bit)
    desiredDisplay[1] = number / 16;              //INSTANTIATE VALUES TO PRINT
    desiredDisplay[2] = number % 16;
    P1OUT |= BIT1;                                //DISABLE SEGMENT 1.1
    P1OUT &= ~(BIT7 | BIT6 | BIT5);               //TURN OFF ALL DIGITS
    digitCounter++;                               //FIGURE OUT NEXT DIGIT
    if(digitCounter < 1 | digitCounter > 3)       //HANDLE WHEN digitCounter = 0 or 4
        digitCounter = 1;
    P1OUT |= digitCounterArray[digitCounter-1];   //DISPLAY DESIRED DIGIT
    
    //DISPLAY 1 DESIRED VALUE
    P2OUT = displays[desiredDisplay[digitCounter-1]];
    if(displaysBIT1[desiredDisplay[digitCounter-1]] == 1)
        P1OUT &= ~BIT1;                           //CLEAR BIT1 IF NEEDED
    
}                                           //LEAVE INTERRUPT
