/*	----------------------------------------------------------------------------
	FILE:			io.c
	PROJECT:		pinguino
	PURPOSE:		Peripheral Remappage and IOs Configuration
	PROGRAMER:		Régis Blanchot <rblanchot@gmail.com>
	FIRST RELEASE:	20 Jun. 2012
	LAST RELEASE:	28 Feb 2013
	----------------------------------------------------------------------------
	CHANGELOG:
	23 Nov 2012 - Régis Blanchot - added PIC18F1220,1320,2455,4455,46j50 support
	07 Dec 2012 - Régis Blanchot - added PIC18F25K50 and PIC18F45K50 support
    05 Oct 2013 - Régis Blanchot - replaced SystemUnlock/SystemLock
                                   with EECON2 = 0x55; EECON2 = 0xAA;]
    28 Feb 2013 - Régis Blanchot - renamed functions
                                   added IO_init()
	----------------------------------------------------------------------------
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	--------------------------------------------------------------------------*/

#ifndef __IO_C
#define __IO_C

#include <pic18fregs.h>
#include <typedef.h>
#include <const.h>
#include <macro.h>
//#include <system.c>

void IO_init(void)
{
    // Initialize everything as an output
    TRISA = 0x00;
    TRISB = 0x00;
    TRISC = 0x00;
    #if defined(__18f4455)  || defined(__18f4550)  || \
        defined(__18f45k50) || defined(__18f46j50) || \
        defined(__18f47j53)
    TRISD = 0x00; 
    TRISE = 0x00; 
    #endif

    // Set everything low
    LATA  = 0x00;
    LATB  = 0x00;
    LATC  = 0x00;
    #if defined(__18f4455)  || defined(__18f4550)  || \
        defined(__18f45k50) || defined(__18f46j50) || \
        defined(__18f47j53)
    LATD  = 0x00; 
    LATE  = 0x00; 
    #endif

    // Set everything low
    PORTA  = 0x00;
    PORTB  = 0x00;
    PORTC  = 0x00;
    #if defined(__18f4455)  || defined(__18f4550)  || \
        defined(__18f45k50) || defined(__18f46j50) || \
        defined(__18f47j53)
    PORTD  = 0x00; 
    PORTE  = 0x00; 
    #endif
}

// All Analog Pins as Digital IOs
void IO_digital(void)
{
    #if defined(__18f2455) || defined(__18f4455) || \
        defined(__18f2550) || defined(__18f4550)

        ADCON1 = 0x0F;				// AN0 to AN12 Digital I/O
        CMCON = 0x07;               // Comparators as Digital I/O

    #elif defined(__18f25k50) || defined(__18f45k50)

        // Initialize all Analog pins as Digital I/O
        ANSELA = 0;
        ANSELB = 0;
        ANSELC = 0;
        #if defined(__18f45k50)
        ANSELD = 0;
        ANSELE = 0;
        #endif

    #elif defined(__18f26j50) || defined(__18f46j50)

        // Initialize all Analog pins as Digital I/O
        ANCON0 = 0xFF;				// AN0 to AN7  Digital I/O
        ANCON1 = 0x1F;				// AN8 to AN12 Digital I/O

        // Turn off all comparators 
        CM1CON = 0x00;
        CM2CON = 0x00; 
        CVRCON = 0x00;

    #endif
}

/**
 * PIC  PINGUINO    PPS
 * 0    RB0         RP3
 * 1    RB1         RP4
 * 2    RB2         RP5
 * 3    RB3         RP6
 * 4    RB4         RP7
 * 5    RB5         RP8
 * 6    RB6         RP9
 * 7    RB7         RP10
 * 8    RC6         RP17
 * 9    RC7         RP18
 * 10   RC0         RP11
 * 11   RC1         RP12
 * 12   RC2         RP13
 * 13   RA0         RP0
 * 14   RA1         RP1
 * 15   RA2         -
 * 16   RA3         -
 * 17   RA5         RP2
 **/
 
// Peripheral Remappage
// NB1 : the Configuration bit IOL1WAY is set to OFF in the bootloader
// NB2 : pins must be explicitly reconfigured as digital I/O when used with a PPS

#if defined(__18f26j50) || defined(__18f46j50)
void IO_remap(void)
{
    #if defined(__18f26j50) || defined(__18f46j50)

        //SystemUnlock();
        EECON2 = 0x55;
        EECON2 = 0xAA;
        PPSCONbits.IOLOCK = 0;			// Turn off PPS Write Protect
        //SystemLock();

        //#ifdef __SERIAL2__
        RPINR16 = 4;                    // RP4 (RB1) <- RX2
        RPOR3 = 5;                      // RP3 (RB0) -> TX2 (func. num. 5)
        //RPINR17 = ;                     // EUSART2 Clock Input (CKR2)
        //#endif

        //#ifdef __SPI2__
        RPINR21 = 6;                    // RP6 (RB3) <- SDI2
        RPOR5 = 10;                     // RP5 (RB2) -> SCK2
        RPOR4 = 9;                      // RP4 (RB1) -> SDO2 (func. num. 9)
        //RPOR3 = 12;                     // RP3 (RB0) -> SS2 (SPI DMA Slave Select)
        //#endif

        //#ifdef __PWM__
        RPOR11 = 14;                    // RP11 (RC0) <- CCP1
                //P1B      - 15 - ECCP1 Enhanced PWM Output, Channel B
                //P1C      - 16 - ECCP1 Enhanced PWM Output, Channel C
                //P1D      - 17 - ECCP1 Enhanced PWM Output, Channel D
        RPOR12 = 18;                    // RP12 (RC1) <- CCP2
                //P2B      - 19 - ECCP2 Enhanced PWM Output, Channel B
                //P2C      - 20 - ECCP2 Enhanced PWM Output, Channel C
                //P2D     - 21 - ECCP2 Enhanced PWM Output, Channel D
        // RPINR24 = ;                     // PWM Fault Input (FLT0)
        
        //#endif

        //SystemUnlock();
        EECON2 = 0x55;
        EECON2 = 0xAA;
        PPSCONbits.IOLOCK = 1;			// Turn on PPS Write Protect
        //SystemLock();

    #endif /* defined(PINGUINO26J50) || defined(PINGUINO46J50) */
}
#endif /* defined(__18f26j50) || defined(__18f46j50) */

#endif /* __IO_C */
