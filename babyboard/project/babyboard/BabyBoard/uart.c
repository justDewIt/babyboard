/* uart.c
 * Authors: Andrew Dudney and Matthew Haney
 * Copyright 2016 Andrew Dudney
 * Provides an implementation to the UART interface defined in uart.h
 */

#include <msp430.h>
#include "uart.h"
#include <string.h>


#define TXLED BIT0
#define RXLED BIT6
#define TXD BIT2
#define RXD BIT1


char character;

// Parity disabled, LSB first, 8 bit data, 1 stop bit

// set_clock: Set the internal clock to the provided speed, in MHz.
void set_clock(int speed)
{
    //Reset clock frequency control
    DCOCTL = 0;

    if(speed == 1)
    {
        //If input speed variable is 1:
        //The Basic Clock System Control is set to 1MHz
        //And the Clock Frequency Control is set to 1MHz
        BCSCTL1 = CALBC1_1MHZ;
        DCOCTL = CALDCO_1MHZ;
    }
    else if(speed == 8)
    {
        //If input speed variable is 8:
        //The Basic Clock System Control is set to 8MHz
        //And the Clock Frequency Control is set to 8MHz
        BCSCTL1 = CALBC1_8MHZ;
        DCOCTL = CALDCO_8MHZ;
    }
    else if(speed == 16)
    {
        //If input speed variable is 16:
        //The Basic Clock System Control is set to 16MHz
        //And the Clock Frequency Control is set to 16MHz
        BCSCTL1 = CALBC1_16MHZ;
        DCOCTL = CALDCO_16MHZ;
    }

}


/* init_uart: Initialize everything necessary for the UART functionality you are implementing.
*  Be sure not to affect pins other than the TX and RX pins (output values, directions, or select registers).
*  You must support a baud rate of 9600 (UART_BAUD_9600) and 115200 (UART_BAUD_115200).  The other baud rates are optional.
*/
void init_uart(char baud, char clk)
{
  /*
   *
   * 1. Set UCSWRST
   * 2. Initialize all USCI registers with UCSWRST = 1 (including UCAxCTL1)
   * 3. Configure ports
   * 4. Clear USCWRST via software
   * 5. (Optional) Enable interrupts via UCAxRXIE and/or UCAxTXIE
   *
   */

    //First set UCSWRST value
    UCA0CTL1 |= UCSWRST;
    //Then set SMCLK uart clock
    UCA0CTL1 |= UCSSEL_2;




    // UC7BIT = 8-bit char data bit;
    // UCMODEx = UART mode;
    // UCSPB = one stop bit;
    // UCSYNC = UART mode


    //AND-ing USCI 'control 0' with NOT VALUES
    UCA0CTL0 &= ~UC7BIT;     //8-bit data
    UCA0CTL0 &= ~UCMODE0;
    UCA0CTL0 &= ~UCMODE1;
    UCA0CTL0 &= ~UCPEN;      //Disable parity bit
    UCA0CTL0 &= ~UCSPB;      //One stop bit
    UCA0CTL0 &= ~UCMSB;      //LSB first
    UCA0CTL0 &= ~UCSYNC;

    P1SEL |= BIT1 | BIT2;
    P1SEL2 |= BIT1 | BIT2;

    //Setting Baud Rate
    switch (baud){

    //Baud = 9600
        case 0:
            if(clk == 1)//For 1MHz at 9600Baud
            {
                //hex version of 104dec = 68
                //Baud Rate control 0
                UCA0BR0 = 0x68;
                //Baud Rate control 1
                UCA0BR1 = 0x00;

                //USCI_A0 Modulation Control:
                //Select 1 for second stage modulation
                UCA0MCTL |= UCBRS_1;

                //Select 0 for first stage modulation
                UCA0MCTL |= UCBRF_0;
            }
            else if (clk == 8)//For 8MHz at 9600Baud
            {
                //hex version of 833dec = 341
                UCA0BR0 = 0x41;
                UCA0BR1 = 0x03;

                //USCI Modulation Control:
                //Select 2 for second stage modulation
                UCA0MCTL |= UCBRS_2;

                //Select 0 for first stage modulation
                UCA0MCTL |= UCBRF_0;
            }
            else if (clk == 16)//For 16MHz at 9600Baud
            {
                //hex version of 1666dec = 682
                UCA0BR0 = 0x82;
                UCA0BR1 = 0x06;

                //USCI_A0 Modulation Control:
                //Select 6 for second stage modulation
                UCA0MCTL |= UCBRS_6;

                //Select 0 for first stage modulation
                UCA0MCTL |= UCBRF_0;
            }
        break;

    //Baud = 115200
        case 1:
            if(clk == 1)//For 1MHz at 115200Baud
            {
                //hex version of 8dec = 08
                UCA0BR0 = 0x08;
                UCA0BR1 = 0x00;

                //USCI_A0 Modulation Control:
                //Select 6 for second stage modulation
                UCA0MCTL |= UCBRS_6;

                //Select 0 for first stage modulation
                UCA0MCTL |= UCBRF_0;
            }
            else if(clk == 8)//For 8MHz at 115200Baud
            {
                //hex version of 69dec = 45
                UCA0BR0 = 0x45;
                UCA0BR1 = 0x00;

                //USCI_A0 Modulation Control:
                //Select 4 for second stage modulation
                UCA0MCTL |= UCBRS_4;

                //Select 0 for first stage modulation
                UCA0MCTL |= UCBRF_0;
            }
            else if(clk == 16)//For 16MHz at 115200Baud
            {
                //hex version of 138dec = 8A
                UCA0BR0 = 0x8A;
                UCA0BR1 = 0x00;

                //USCI_A0 Modulation Control:
                //Select 7 for second stage modulation
                UCA0MCTL |= UCBRS_7;

                //Select 0 for first stage modulation
                UCA0MCTL |= UCBRF_0;
            }

        default:
        break;

    }

    //Need to clear UCSWRST
    UCA0CTL1 &= ~ UCSWRST;

    //Need to enable the RX interrupt
    UC0IE |= UCA0RXIE;


   // __enable_interrupt();
   // __bis_SR_register(GIE);
}

// uninit_uart: Uninitialize the uart driver.
void uninit_uart()
{
    //Reset SMCLK clock
    UCA0CTL1 &= ~UCSSEL_2;
    //Reset UCSWRST
    UCA0CTL1 |= UCSWRST;

    //USCI_A0 Modulation Control:
    //Select 0 for second stage modulation
    UCA0MCTL = UCBRS_0;
    //Select 0 for second stage modulation
    UCA0MCTL = UCBRF_0;

    //Reset Control Register 0
    UCA0CTL0 = 0;
}

// putch: Send an unsigned char via UART.  This function should transmit characters correctly regardless of how many times this function is called in rapid succession.
void putch(unsigned char c)
{
   //Can be found on page 433 in datasheet
    while(!(IFG2 & UCA0TXIFG));

    //Put character to be sent into transmit buffer
    UCA0TXBUF = (char) c;

    return;
}

// put_str: Send each element of a null-terminated array of unsigned chars via UART.  Do not send the final null-termination character.
void put_str(unsigned char* c)
{
    //counter for while loop
    int i = 0;
    //Size of string
    int size = strlen((char*)c);

    //While loop runs through every character in string
    //Sends through putch() with one character strings at a time

    while(i < size)
    {
        putch(*(c + i));
        i++;
    }

    return;
}

/*  uart_rx: Return the most recent character received via UART.
*   The block parameter determines the behavior of uart_rx if no character has been received.  The functionality is defined as follows:
*   If a character has been received, return the most recently received character.
*   If no character has been received and block is set to zero, return -1.
*   If no character has been received and block is set to one, wait for a character to be received and then return that character.
*   Thus, if the microcontroller receives 'a' one time, and this function is called twice with block = 0, the first call should return 'a' and the second should return -1.
*   If the microcontroller receives 'a' one time, and this function is called twice with block = 1, the first call should return 'a' and the
*   second should wait indefinitely until a character is received, and then return that character.
*/

char uart_rx(char block)
{
    //Set character equal to zero
    character = 0;

    //Wait indefinitely until a character is received
    while(character == 0);

    UCA0TXBUF = character;

    //Return character value
    return character;

}



#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
    character = UCA0RXBUF;
    UCA0TXBUF = character;
}


