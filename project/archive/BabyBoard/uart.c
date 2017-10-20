/* uart.c
 * Authors: Andrew Dudney and Matthew Haney
 * Copyright 2016 Andrew Dudney
 * Provides an implementation to the UART interface defined in uart.h
 */

#include <msp430.h>
#include "uart.h"
#include <string.h>

char character;


// Parity disabled, LSB first, 8 bit data, 1 stop bit


/* -----------------------------------------------------------------
 *                          NOTES ABOUT CLOCKS
 * ------------------------------------------------------------------
 * MCLK:
 *      - Drives the CPU and DMA.
 *      - Driven by internal digitally controlled RC-oscillator (DCO).
 *      - Software selectable as LFXT1CLK, VLOCLK, XT2CLK or DCOCLK.
 *      - MCLK is divided by 1, 2, 4 or 8.
 * SMCLK:
 *      - Used for peripherals, usually synchronous to MCLK.
 *      - Driven by internal digitally controlled RC-oscillator (DCO).
 *      - Software selectable as LFXT1CLK, VLOCLK, XT2CLK or DCOCLK.
 *      - Software selectable for individual peripheral modules.
 *      - SMCLK is divided by 1, 2, 4 or 8.
 *
 *
 * DCOCLK: Internal digitally controlled oscillator (DCO).
 *      - Can be adjusted by software using the DCOx, MODx, RSELx bits
 *      - Disabling the DCOCLK by setting SCG0.
 *
 *
 * The DCOCTL, BCSCTL1, BCSCTL2, and BCSCTL3 registers configure the basic clock module+.
 *
 *
 */

// set_clock: Set the internal clock to the provided speed, in MHz.
void set_clock(int speed) {

    //Reset clock frequency control
    DCOCTL = 0;

    switch (speed){

        //For 1 MHZ speed
        case 1:
            BCSCTL1 = CALBC1_1MHZ;     //Basic clock system control = 1MHZ
            DCOCTL = CALDCO_1MHZ;      //Clock frequency control = 1MHZ
        break;

        //For 8 MHZ speed
        case 8:
            BCSCTL1 = CALBC1_8MHZ;     //Basic clock system control = 8MHZ
            DCOCTL = CALDCO_8MHZ;      //Clock frequency control = 8MHZ
        break;

        //For 16 MHZ speed
        case 16:
            BCSCTL1 = CALBC1_16MHZ;    //Basic clock system control = 16MHZ
            DCOCTL = CALDCO_16MHZ;     //Clock frequency control = 16MHZ
        break;

        default:
        break;

    }
}

/* init_uart: Initialize everything necessary for the UART functionality you are implementing.
*  Be sure not to affect pins other than the TX and RX pins (output values, directions, or select registers).
*  You must support a baud rate of 9600 (UART_BAUD_9600) and 115200 (UART_BAUD_115200).  The other baud rates are optional.
*/
void init_uart(char baud, char clk) {
  /*
   *
   * 1. Set UCSWRST
   * 2. Initialize all USCI registers with UCSWRST = 1 (including UCAxCTL1)
   * 3. Configure ports
   * 4. Clear USCWRST via software
   * 5. (Optional) Enable interrupts via UCAxRXIE and/or UCAxTXIE
   *
   */

    //bauds[2][3];

    UCA0CTL1 |= UCSWRST;    //set UCSWRST
    UCA0CTL1 |= UCSSEL_2;   //SMCLK clock  "SMCLK for UART"

    /*8 bit char data
     * UC7BIT = 8-bit char data bit;
     * UCMODEx = UART mode;
     * UCSPB = one stop bit;
     * UCSYNC = UART mode
     * */


    UCA0CTL0 &= ~UC7BIT;     //8-bit data
    UCA0CTL0 &= ~UCMODE0;
    UCA0CTL0 &= ~UCMODE1;
    UCA0CTL0 &= ~UCPEN;      //Disable parity bit
    UCA0CTL0 &= ~UCSPB;      //One stop bit
    UCA0CTL0 &= ~UCMSB;      //LSB first
    UCA0CTL0 &= ~UCSYNC;



    /*
     *Baud rate set
     *Case statement for baud rates.
     *Case values from uart.h
     */
    switch (baud){

        //TABLE ON PAGE 424 data sheet

        //Baud = 9600
        case 0:
            switch(clk){

                //1MHz, 9600 Baud
                case 1:
                    UCA0BR0 = 0x68;     //104 decimal = 68 in hex
                    UCA0BR1 = 0x00;
                    UCA0MCTL |= UCBRS_1;  //Modulation control, set to Second Stage Modulation Select 2
                    UCA0MCTL |= UCBRF_0;  //Modulation control, USCI First Stage Modulation Select 0
                break;

                //8MHz, 9600 Baud
                case 8:
                    UCA0BR0 = 0x41;     //833 decimal = 341 in hex
                    UCA0BR1 = 0x03;
                    UCA0MCTL |= UCBRS_2;  //Modulation control, set to Second Stage Modulation Select 2
                    UCA0MCTL |= UCBRF_0;  //Modulation control, USCI First Stage Modulation Select 0
                break;

                //16MHz, 9600 Baud
                case 16:
                    UCA0BR0 = 0x82;     //1666 decimal = 682 in hex
                    UCA0BR1 = 0x06;
                    UCA0MCTL |= UCBRS_6;  //Modulation control, set to Second Stage Modulation Select 6
                    UCA0MCTL |= UCBRF_0;  //Modulation control, USCI First Stage Modulation Select 0
                break;

                default:
                break;
            }
        break;

        //Baud = 19200
        //case 1:
            /*UCA0BR0 = 0x36;     //54 decimal = 36 in hex
            UCA0BR1 = 0x00;
            UCA0MCTL |= UCBRS_5;  //Modulation control, set to Second Stage Modulation Select 5
            UCA0MCTL |= UCBRF_0;*/  //Modulation control, USCI First Stage Modulation Select 0
        //break;

        //Baud = 38400
        //case 2:
            /*UCA0BR0 = 0x1B;     //27 decimal = 1B in hex
            UCA0BR1 = 0x00;
            UCA0MCTL |= UCBRS_2;  //Modulation control, set to Second Stage Modulation Select 2
            UCA0MCTL |= UCBRF_0;*/  //Modulation control, USCI First Stage Modulation Select 0
        //break;

        //Baud = 56000
        /*case 3:
            UCA0BR0 = 0x12;     //18 decimal = 12 in hex
            UCA0BR1 = 0x00;
            UCA0MCTL |= UCBRS_6;  //Modulation control, set to Second Stage Modulation Select 6
            UCA0MCTL |= UCBRF_0;  //Modulation control, USCI First Stage Modulation Select 0
        break;*/

        //Baud = 115200
        case 1:
            switch(clk){

                //1 MHz, 115200 Baud
                case 1:
                    UCA0BR0 = 0x08;     //8 decimal = 08 in hex
                    UCA0BR1 = 0x00;
                    UCA0MCTL |= UCBRS_6;  //Modulation control, set to Second Stage Modulation Select 6
                    UCA0MCTL |= UCBRF_0;  //Modulation control, USCI First Stage Modulation Select 0
                break;

                //8 MHz, 115200 Baud
                case 8:
                    UCA0BR0 = 0x45;     //69 decimal = 45 in hex
                    UCA0BR1 = 0x00;
                    UCA0MCTL |= UCBRS_4;  //Modulation control, set to Second Stage Modulation Select 4
                    UCA0MCTL |= UCBRF_0;  //Modulation control, USCI First Stage Modulation Select 0
                break;

                //16 MHz, 115200 Baud
                case 16:
                    UCA0BR0 = 0x8A;     //138 decimal = 8A in hex
                    UCA0BR1 = 0x00;
                    UCA0MCTL |= UCBRS_7;  //Modulation control, set to Second Stage Modulation Select 7
                    UCA0MCTL |= UCBRF_0;  //Modulation control, USCI First Stage Modulation Select 0
                break;

                default:
                    break;
            }
        //break;

        default:
        break;

    }

    UCA0CTL1 &= ~ UCSWRST;  //clear UCSWRST

    UC0IE |= UCA0RXIE;    //Enable RX interrupt
    //__enable_interrupt();
    //__bis_SR_register(GIE);


}

// uninit_uart: Uninitialize the uart driver.
void uninit_uart() {

    UCA0CTL1 &= ~UCSSEL_2;   //SMCLK clock **RESET**

    UCA0CTL1 |= UCSWRST;    //reset UCSWRST
    UCA0MCTL = UCBRS_0;     //Modulation control, set to Second Stage Modulation Select 0
    UCA0MCTL = UCBRF_0;     //Modulation control, USCI First Stage Modulation Select 0
    UCA0CTL0 = 0;           //reset control register 0

}

// putch: Send an unsigned char via UART.  This function should transmit characters correctly regardless of how many times this function is called in rapid succession.
void putch(unsigned char c) {
    while(!(IFG2 & UCA0TXIFG)); //pg 433.
    UCA0TXBUF = (char) c; //Puts the character that needs sending into the transmit buffer.
    return;
}

// put_str: Send each element of a null-terminated array of unsigned chars via UART.  Do not send the final null-termination character.
void put_str(unsigned char* c) {
    //putch('a');
    int i = 0;                      //Initialize counter because for loops give errors
    int size = strlen((char*)c);    //Create a variable that contains the size of the string

    //Goes through each character in the string and sends it through the putch function. Sends strings 1 character a time.
    while(i < size){
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

char uart_rx(char block) {
  character = 0;
  while(character == 0);
  UCA0TXBUF = character;
  return character;

}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
    character = UCA0RXBUF;
    UCA0TXBUF = character;
}
