/*******************************************************************************
	Title:	USB Pinguino Bootloader
	File:	hardware.h
	Descr.: bootloader def. (version, led, tempo.)
	Author:	Andr� Gentric
			R�gis Blanchot <rblanchot@gmail.com>
			Based on Albert Faber's JAL bootloader
			and Alexander Enzmann's USB Framework
	This file is part of Pinguino (http://www.pinguino.cc)
	Released under the LGPL license (http://www.gnu.org/licenses/lgpl.html)
*******************************************************************************/

#include <pic18fregs.h>
#include "types.h"
#include "hardware.h"
#include "picUSB.h"
#include "config.h"

__code USB_Device_Descriptor device_descriptor = 
{
    sizeof(USB_Device_Descriptor),              // Size of this descriptor in bytes
    DEVICE_DESCRIPTOR,                          // Device descriptor type
    0x0200,                                     // USB Spec Release Number in BCD format (0x0100 for USB 1.0, 0x0110 for USB1.1, 0x0200 for USB2.0)
    0xff,                                       // Class Code-->00
    0x00,                                       // Subclass code
    0xff,                                       // Protocol code
    EP0_BUFFER_SIZE,                            // Max packet size for EP0
    0x04D8,                                     // Vendor ID, microchip=0x04D8, generic=0x05f9, test=0x067b
    0xFEAA,                                     // Product ID 0x00A f�r CDC, generic=0xffff, test=0x2303
    (MAJOR_VERSION<<8)+MINOR_VERSION,           // Device release number in BCD format-->0
    1,                                          // Manufacturer string index (0=no string descriptor)
    2,                                          // Product string index (0=no string descriptor)
    0,                                          // Device serial number string index
    1                                           // Number of possible configurations
};

__code USB_Configuration_Descriptor configuration_descriptor =
{
    // Configuration Descriptor Header
    {sizeof(USB_Configuration_Descriptor_Header),// Size of this descriptor in bytes
    CONFIGURATION_DESCRIPTOR,                   // CONFIGURATION descriptor type
    sizeof(USB_Configuration_Descriptor),       // Total length of data for this configuration
    1,                                          // Number of interfaces in this configuration
    1,                                          // Index value of this configuration
    0,                                          // Configuration string index
    192,//DEFAULT | POWERED,                          // Attributes
    20},                                         // Maximum Power Consumption in 2mA units
    // Data Interface Descriptor with in and out EPs
    {sizeof(USB_Interface_Descriptor),          // Size of this descriptor in bytes
    INTERFACE_DESCRIPTOR,                       // Interface descriptor type
    0,                                          // Interface Number
    0,                                          // Alternate Setting Number
    2,                                          // Number of endpoints in this interface
    0xff,                                       // Class code
    0xff,                                       // TODO: Subclass code
    0xff,                                       // TODO: Protocol code
    0},                                         // Index of String Descriptor Describing this interface-->2
    // Endpoint 1 Out
    {sizeof(USB_Endpoint_Descriptor),           // Size of Descriptor
    ENDPOINT_DESCRIPTOR,                        // Descriptor Type
    0x01,                                       // Endpoint Address
    0x02,                                       // Attribute = Bulk Transfer
    EP1_BUFFER_SIZE,                            // Packet Size
    0x00},                                      // Poll Intervall
    // Endpoint 1 IN
    {sizeof(USB_Endpoint_Descriptor),           // Size of Descriptor
    ENDPOINT_DESCRIPTOR,                        // Descriptor Type
    0x81,                                       // Endpoint Address
    0x02,                                       // Attribute = Bulk Transfer
    EP1_BUFFER_SIZE,                            // Packet Size
    0x00}
};

const char lang[] = {sizeof(lang),  STRING_DESCRIPTOR, 0x09,0x04}; // english = 0x0409
const char manu[] = {sizeof(manu),  STRING_DESCRIPTOR,
    'R',0x00,'.',0x00,'B',0x00,'l',0x00,'a',0x00,'n',0x00,'c',0x00,'h',0x00,'o',0x00,'t',0x00};
//  '/',0x00,
//  'G',0x00,'e',0x00,'n',0x00,'t',0x00,'r',0x00,'i',0x00,'c',0x00,};
const char prod[] = {sizeof(prod),  STRING_DESCRIPTOR,
    'P',0x00,'i',0x00,'n',0x00,'g',0x00,'u',0x00,'i',0x00,'n',0x00,'o',0x00};
const char * const string_descriptor[] = { lang, manu, prod};

/*  --------------------------------------------------------------------
    ------------------------------------------------------------------*/
    
void delay(void) __naked
{
    __asm
        movlw	0xFF
        movwf	r0x00
        movlw	0xFF
        movwf	r0x01
    startup_loop:
        decfsz	r0x00, f
        bra 	startup_loop
        decfsz	r0x01, f
        bra 	startup_loop
    __endasm;
}

/*  --------------------------------------------------------------------
    ------------------------------------------------------------------*/
    
void start_write(void) __naked
{
	__asm
    
	#if defined(__18f2455)  || defined(__18f4455) || \
	    defined(__18f2550)  || defined(__18f4550) || \
        defined(__18f25k50) || defined(__18f45k50)

        ;bcf     _INTCON, 7      ; INTCONbits.GIE = 0 ; disable interrupts (activated in startup sequence)
        movlw	0x55
        movwf	_EECON2			; EECON2 = 0x55;
        movlw	0xAA
        movwf	_EECON2			; EECON2 = 0xAA;
        bsf		_EECON1, 1		; EECON1bits.WR = 1; start flash/eeprom writing
                                ; CPU stall here for 2ms
        ;bsf     _INTCON, 7      ; INTCONbits.GIE = 1 ; re-enable interrupts
        ;nop						; proc. can forget to execute the first operation

    #elif defined(__18f26j50) || defined(__18f46j50) || \
          defined(__18f14k50)

		;bsf     _EECON1, 5      ; EECON1bits.WPROG = 1;	Program 2 bytes on the next WR command
        bsf		_EECON1, 2		; EECON1bits.WREN = 1; allows write cycles to Flash program memory
        ;bcf     _INTCON, 7      ; INTCONbits.GIE = 0 ; disable interrupts (activated in startup sequence)
        movlw	0x55
        movwf	_EECON2			; EECON2 = 0x55;
        movlw	0xAA
        movwf	_EECON2			; EECON2 = 0xAA;
        bsf		_EECON1, 1		; EECON1bits.WR = 1; start flash/eeprom writing
        bcf		_EECON1, 2		; EECON1bits.WREN = 0; inhibits write cycles to Flash program memory
        ;bsf     _INTCON, 7      ; INTCONbits.GIE = 1 ; re-enable interrupts

	#endif

	__endasm;
}

/*  --------------------------------------------------------------------
    ------------------------------------------------------------------*/
    
void disable_boot(void) __naked
{
	__asm
        clrf	_T1CON				; disable timer 1
        clrf	_UCON				; disable USB
        bsf		LED_TRIS, LED_PIN	; led input
        bcf		LED_PORT, LED_PIN	; led off
        call    _delay              ; force timeout on USB
	__endasm;
}

/*  --------------------------------------------------------------------
	UEP1bits.EPHSHK   = 1;		// EP handshaking on
	UEP1bits.EPCONDIS = 1;		// control transfers off
	UEP1bits.EPOUTEN  = 1;		// EP OUT enabled
	UEP1bits.EPINEN   = 1;		// EP IN enabled
    ------------------------------------------------------------------*/
    
void usb_configure_endpoints()
{
    UEP1 = 0b00011110;
    
	// for IN
	// set DTS bit, turn on data togle sync TOGGLE
	EP_IN_BD(1).Stat.uc  = 0b01000000;

	// for OUT
	EP_OUT_BD(1).Cnt  = EP1_BUFFER_SIZE;
	EP_OUT_BD(1).ADDR = PTR16(&bootCmd);
	// set UOWN bit, SIE owns the buffer
	EP_OUT_BD(1).Stat.uc = 0b10000000;
}

/*  --------------------------------------------------------------------
    ------------------------------------------------------------------*/
    
void usb_ep_data_out_callback(char end_point)
{
	byte counter;

	// whatever the command, keep LED high
	__asm
	bsf		LED_PORT, LED_PIN	; led on
	__endasm;
 
    // Number of byte(s) to return
	EP_IN_BD(end_point).Cnt = 0;

    // load table pointer
    TBLPTRU = bootCmd.addru;
    TBLPTRH = bootCmd.addrh;
    TBLPTRL = bootCmd.addrl;

/**********************************************************************/
	if (bootCmd.cmd ==  RESET)
/**********************************************************************/
	{
		disable_boot();
		__asm
		goto	ENTRY		; start user app
		__endasm;
	}
/**********************************************************************/
	else if (bootCmd.cmd == READ_VERSION)
/**********************************************************************/
	{
		bootCmd.buffer[2] = MINOR_VERSION;
		bootCmd.buffer[3] = MAJOR_VERSION;
        // Number of byte(s) to return
		EP_IN_BD(end_point).Cnt = 4;
		//T1CON = 0b00000000; // disable timer 1 ???
	}

/**********************************************************************/
	else if (bootCmd.cmd == READ_FLASH)
/**********************************************************************/
	{
		for (counter=0; counter < bootCmd.len; counter++)
		{
			__asm TBLRD*+ __endasm;
			bootCmd.xdat[counter] = TABLAT;
		}

        // Number of byte(s) to return
		EP_IN_BD(end_point).Cnt = 5 + bootCmd.len;
	}

/**********************************************************************/
	else if (bootCmd.cmd == WRITE_FLASH)
/**********************************************************************/
	{
        #if defined(__18f2550)  || defined(__18f4550)  || \
            defined(__18f2455)  || defined(__18f4455)  || \
            defined(__18f25k50) || defined(__18f45k50) || \
            defined(__18f14k50)

        /// The programming block is 32 bytes for all chips except x5k50
        /// The programming block is 64 bytes for x5k50.
        /// It is not necessary to load all holding register before a write opreration
        /// Word or byte programming is not supported by these chips.
        /// NB:
        /// * High Speed USB has a Max. packet size of 64 bytes
        /// * Uploader (uploader8.py) sends 32-byte Data block + 5-byte Command block 

        //TBLPTRL = (TBLPTRL & 0xF0);   // Force 16-byte boundary
        //TBLPTRL = (TBLPTRL & 0xE0);   // Force 32-byte boundary
        // Load max. 32 holding registers
		for (counter=0; counter < bootCmd.len; counter++)
		{
			TABLAT = bootCmd.xdat[counter];     // present data to table latch
			__asm TBLWT*+ __endasm;             // write data in TBLWT holding register
        }
        __asm TBLRD*- __endasm;                 // to be inside the 32 bytes range
        // issue the block
		EECON1 = 0b10000100; // allows write (WREN=1) in flash (EEPGD=1)
		start_write();

        #elif defined(__18f26j50) || defined(__18f46j50)
        
        /// blocks must be erased before written
        /// the whole memory is erased at the begining of upload
        /// we write 32 bytes not 64 bytes
        /// n   : write [address]      + 64 bytes
        /// n+1 : write [address + 32] + 64 bytes
        /// 32 bytes are written 2 times
        /// that's why we use 2-byte write instead of 64-byte write
        
		EECON1bits.WPROG = 1; // Enable single-word write
		for (counter=0; counter < bootCmd.len; counter+=2)
		{
			TBLPTRL =  bootCmd.addrl + counter;
			TABLAT = bootCmd.xdat[counter];
            // TBLPTR is incremented after the write
			__asm TBLWT*+ __endasm;
			TABLAT = bootCmd.xdat[counter + 1];
            // TBLPTR is NOT incremented after the write
			__asm TBLWT* __endasm;
			start_write();
		}
		EECON1bits.WPROG = 0;	// Disable single-word write

        #endif

        // Number of byte(s) to return
		EP_IN_BD(end_point).Cnt = 1;
	}

/**********************************************************************/
	else if (bootCmd.cmd ==  ERASE_FLASH)
/**********************************************************************/
	{

        #if defined(__18f2550)  || defined(__18f4550)  || \
            defined(__18f2455)  || defined(__18f4455)  || \
            defined(__18f25k50) || defined(__18f45k50) || \
            defined(__18f14k50)
        
		// The erase block is 64 bytes
        // bootCmd.len = num. of 64-byte block to erase
        
		EECON1 = 0b10010100; // allows erase (WREN=1, FREE=1) in flash (EEPGD=1)
		for (counter=0; counter < bootCmd.len; counter++)
		{
			start_write();
			__asm
			movlw	0x40
			addwf	_TBLPTRL			; TBLPTR += 64
			movlw	0x00				; if (TBLPTRL == 0)
			addwfc	_TBLPTRH			; TBLPTRH += 1
			__endasm;
		}
		// TBLPTRU = 0

        #elif defined(__18f26j50) || defined(__18f46j50)

		// The erase block is 1024 bytes
        // bootCmd.len = num. of 1024-byte blocks to erase

		for (counter=0; counter < bootCmd.len; counter++)
		{
            EECON1bits.FREE = 1;    // allow a program memory erase operation
            start_write();
            EECON1bits.FREE = 0;    // inhibit program memory erase operation

            // next block (TBLPTR = TBLPTR + 1024)
			__asm
			movlw	0x04                ; 0x04 + (TBLPTRH) -> TBLPTRH
			addwf	_TBLPTRH, 1			;  (W) + (TBLPTRH) -> TBLPTRH
                                        ; (C) is affected
			movlw	0x00				; 0x00 + (TBLPTRU) + (C) -> TBLPTRU
			addwfc	_TBLPTRU, 1			;  (W) + (TBLPTRU) + (C) -> TBLPTRU
			__endasm;
		}

        #endif

        // Number of byte(s) to return
		EP_IN_BD(end_point).Cnt = 1;
	}

/**********************************************************************/
    // Is there something to return ?
	if (EP_IN_BD(end_point).Cnt > 0)
	{
        // Data packet toggle
		if (EP_IN_BD(1).Stat.DTS)
			EP_IN_BD(1).Stat.uc = 0b10001000; // UOWN 1 DTS 0 DTSEN 1
		else
			EP_IN_BD(1).Stat.uc = 0b11001000; // UOWN 1 DTS 1 DTSEN 1
	}

	// reset size
    EP_OUT_BD(end_point).Cnt = EP1_BUFFER_SIZE;
	// set to UOWN
    EP_OUT_BD(end_point).Stat.uc = 0x80;      // UOWN 1
}

/*  --------------------------------------------------------------------
    Main loop
    ------------------------------------------------------------------*/
 
void main(void) __naked
{
	dword i = 0;
    byte t1_count = 0;
	word led_counter = 0;

	__asm

    //bcf     _PIR2, 4            ; Clear USB Interrupt Flag

/**********************************************************************/
    #if defined(__18f26j50) || defined(__18f46j50)
/**********************************************************************/
        bsf     _OSCTUNEbits, 6     ; Enable the PLL (PLLEN=bit6)
        call    _delay              ; Wait 2+ms until the PLL locks
                                    ; before enabling USB module
        movlw	0xFF
        movwf	_ANCON0				; all I/O to Digital mode
        movlw	0x1F
        movwf	_ANCON1				; all I/O to Digital mode
/**********************************************************************/
    #elif defined(__18f25k50) || defined(__18f45k50)
/**********************************************************************/
        movlw   0x70                ; 0b01110000 : 111 = HFINTOSC (16 MHz)
        movwf   _OSCCON             ; enable the 16MHz internal clock
    wait_hfintosc:
        btfss   _OSCCON, 2          ; HFIOFS: HFINTOSC Frequency Stable bit
        bra     wait_hfintosc       ; wait HFINTOSC frequency is stable (HFIOFS=1) 
        clrf	_ANSELA				; all I/O to Digital mode
        clrf	_ANSELB				; all I/O to Digital mode
        clrf	_ANSELC				; all I/O to Digital mode
        #if defined(__18f45k50)
            clrf	_ANSELD			; all I/O to Digital mode
            clrf	_ANSELE			; all I/O to Digital mode
        #endif
/**********************************************************************/
    #elif defined(__18f2550)  || defined(__18f4550)  || \
          defined(__18f2455)  || defined(__18f4455)  || \
          defined(__18f14k50)
/**********************************************************************/
;        movlw	0x0F
;        movwf	_ADCON1				; all I/O to Digital mode
;        movlw	0x07
;        movwf	_CMCON				; all I/O to Digital mode
/**********************************************************************/
    #else
/**********************************************************************/
        #error "    --------------------------    "
        #error "    PIC NO YET SUPPORTED !        "
        #error "    Please contact developers.    "
        #error "    --------------------------    "

    #endif
/**********************************************************************/

    bcf		LED_TRIS, LED_PIN	; led output
	bsf		LED_PORT, LED_PIN	; led on

    ;bcf     _PIR1, 0            ; PIR1bits.TMR1IF = 0;
	movlw	b'00110001'			; prescaler 8 (0b11)
	movwf	_T1CON				; timer 1 on, 

	;bsf		_INTCON, 6			; Enable Peripheral interrupt (PEIE=bit6)
	;bsf		_INTCON, 7			; Enable General interrupt (GIE=bit7)

	clrf	_EECON1 			; EECON1=0

	#if (SPEED == LOW_SPEED)
        movlw	b'00010000'		; (0x10) Enable pullup resistors and low speed mode
	#else
        movlw	b'00010100'		; (0x14) Enable pullup resistors and full speed mode
	#endif
    
	banksel	_UCFG
	movwf	_UCFG, b

	__endasm;

	// Initialize USB

	EP_IN_BD(1).ADDR = PTR16(&bootCmd);
	currentConfiguration = 0x00;
	deviceState = DETACHED;

    // Non-blocking loop if USB cable is not connected (ext. supply)
	do {
		EnableUSBModule();
		ProcessUSBTransactions();
        i = i + 1;
        if (i == 0xFFFFF) break; 
    } while (deviceState != CONFIGURED);
    
    // If no USB cable then start user app. now
    if (deviceState != CONFIGURED)
    {
        t1_count = BOOT_TIMER_TICS;
        __asm
            bcf		LED_PORT, LED_PIN	; led on
        __endasm;
    }

	while (1)
	{
		//if (i!=0)
            ProcessUSBTransactions();

		// strobing LED
		if (led_counter == 0)
		{
			__asm
			movlw	LED_MASK	; toggle
			xorwf	LED_PORT, f	; the led
			__endasm;
		}
		led_counter++;

		// timeout ?
		if (PIR1bits.TMR1IF == 1)
		{
			t1_count++;
			PIR1bits.TMR1IF = 0;

			// if expired, then jump to user location
			if (t1_count > BOOT_TIMER_TICS)
			{
				disable_boot();
				__asm
				goto	ENTRY		; start user app
				__endasm;
			}
		}
	}
}

/***********************************************************************
    Jumps must be declared here so :
    1/ we can simply use ENTRY (impossible in crt0Boot4.c #pragma)
    2/ avoid extra file manipulation, just need to change ENTRY in Makefile
    3/ Never use --ivt-loc option as it will also move the Reset vector from 0 to ENTRY
***********************************************************************/

// 0x0000
void reset_isr(void) __naked __interrupt 0
{
    __asm
    goto    _main
    ;goto    _startup
    __endasm;
}

// 0x0008
void high_priority_isr(void) __naked __interrupt 1
{
    __asm
    goto	ENTRY + 0x08
    __endasm;
}

// 0x0018
void low_priority_isr(void) __naked __interrupt 2
{
    __asm
    goto	ENTRY + 0x18
    __endasm;
}

// C stack init.
/*
void startup(void) __naked
{
    __asm
    ; Initialize the stack pointer
    ;lfsr    1, _stack_end
    ;lfsr    2, _stack_end

    ; 1st silicon does not do this on POR
    ;clrf    _TBLPTRU, 0

    ; Initialize the flash memory access configuration.
    ;bsf     0xa6, 7, 0      ; EECON1.EEPGD = 1, TBLPTR accesses program memory or eeprom
    ;bcf     0xa6, 6, 0      ; EECON1.CFGS  = 0, TBLPTR accesses program memory not config. reg.

    goto    _main
    __endasm;
}
*/