// DigiFilter
//
// (c)Ron Taylor G4GXO
//
// Delay Routines
//
// Two delay routines allow delays from 1 microsecond to 10 seconds. The delays are not precise and will
// be extended by a few machine cycles due to call, return and general intialisation times. An additional
// delay will be incurred where an interrupt occurs during a delay period.
// Use the following syntax to call a delay from within a program;
//
// delay_us(delay time in microseconds) e.g. delay_us(100);
// delay_ms(delay time in milliseconds) e.g. delay_ms(100);
//

#include <xc.h>
#include "Main.h"
#include "delays.h"               // Header (before library)
#include <libpic30.h>             // Libraries 
  

// Microsecond delay routine (1 to 1000 uSec)
//
void delay_us(unsigned int uSec){

	if((!uSec)){			// If a zero delay value is used, set delay to 1
		uSec=1;			// to prevent a roll over count
	}

	if (uSec>1000){			// If we exceed 1000uSec
		uSec=1000;		// Hold count at 1000uSec
	}
    __delay_us(uSec);
}

// Millisecond delay routine (1 to 10,000mSec)
//
void delay_ms(unsigned int mSec){

	if((!mSec)){				// If a zero delay value is used, set delay to 1
		mSec=1;                         // to prevent a roll over count
	}

	if (mSec>10000){			// If we exceed 10,000mSec
		mSec=10000;			// Hold count at 10,000mSec
	}
  __delay_ms(mSec);
}

void debounce(){
    __delay_ms(100);
}
