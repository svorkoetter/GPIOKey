/* gpiokey - GPIO Setup and Scanning

   Copyright (c) 2019 by Stefan Vorkoetter

   This file is part of gpiokey.

   gpiokey is free software: you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation, either version 3 of the License, or (at your option)
   any later version.

   gpiokey is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
   details.

   You should have received a copy of the GNU General Public License along with
   gpiokey. If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <fcntl.h>
#include <unistd.h>

#include "x11.h"

#define NUM_PINS 32

static bool initialized = false;

static struct pinMap {
    bool neg;
    KeyCode keyCode;
    bool pressed;
} pinMap[NUM_PINS];

bool ConfigureInputPin( int pin, bool neg, int pud, const char *key,
		        const char **err )
{
    if( pin < 0 || pin > 31 || *key == '\0' ) {
	*err = "pin number out of range";
        return( false );
    }

    if( !initialized ) {
	for( int i = 0; i < NUM_PINS; ++i ) {
	    pinMap[i].neg = false;
	    pinMap[i].keyCode = 0;
	    pinMap[i].pressed = false;
	}
	initialized = true;
    }

    if( pinMap[pin].keyCode )
        fprintf(stderr,"gpiokey: warning, redefining GPIO pin %d\n",pin);

    pinMap[pin].keyCode = StringToKeyCode(key,err);
    if( pinMap[pin].keyCode == 0 )
        return( false );

    pinMap[pin].neg = neg;
    pinMap[pin].pressed = false;

    char cmd[100];
    sprintf(cmd,"gpio export %d in",pin);
    system(cmd);
    sprintf(cmd,"gpio -g mode %d %s",pin,
	    pud == 0 ? "tri" : pud > 0 ? "up" : "down");
    system(cmd);

    return( true );
}

void ScanInputPins( void )
{
    for( int pin = 0; pin < NUM_PINS; ++pin ) {
	if( pinMap[pin].keyCode ) {
	    char path[100];
	    sprintf(path,"/sys/class/gpio/gpio%d/value",pin);
	    int fd = open(path,O_RDONLY);
	    if( fd >= 0 ) {
		char value;
		if( read(fd,&value,1) == 1 ) {
		    bool pressed = (value == '1') ^ pinMap[pin].neg;
		    if( pressed != pinMap[pin].pressed ) {
			pinMap[pin].pressed = pressed;
			SendKeyCode(pinMap[pin].keyCode,pressed);
		    }
		}
	        close(fd);
	    }
	}
    }
}

void RestoreGPIO( void )
{
    /* Turn off any pull up/down resistors we turned on. */
    for( int pin = 0; pin < NUM_PINS; ++pin ) {
        if( pinMap[pin].keyCode ) {
	    char cmd[100];
	    sprintf(cmd,"gpio -g mode %d tri",pin);
	    system(cmd);
	    sprintf(cmd,"gpio unexport %d",pin);
	    system(cmd);
	}
    }
}
