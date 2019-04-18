/* gpiokey - Main Program - Monitors GPIO inputs and turns them into fake
                            keyboard events.

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

#define _BSD_SOURCE	// To enable daemon and usleep functions.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>

#include "gpio.h"
#include "x11.h"

static void usage( FILE *fp )
{
    /* Print usage information and exit. */
    fprintf(fp,"Usage: gpiokey [-b min] [-i key] [-n] [-h] { gpioPin [~] [+|-] keySym }\n\n");
    fprintf(fp,"-b min\t- blank screen after this many minutes of idle time (1 to 10000)\n");
    fprintf(fp,"-i key\t- send the specified key for GPIO events occurring when idle\n");
    fprintf(fp,"-n\t- do not become a daemon, remain in the foreground\n");
    fprintf(fp,"-h\t- display this help\n\n");
    fprintf(fp,"gpioPin\t- BCM GPIO pin number (not physical or WiringPi) to scan for input\n");
    fprintf(fp,"~\t- indicates input is negative logic (active-low)\n");
    fprintf(fp,"+ or -\t- activates internal pull-up or -down resistor\n");
    fprintf(fp,"keySym\t- specifies X11 key symbol associated with the pin\n");
    if( fp == stderr )
    exit(1);
}

int main( int argc, char **argv )
{
    const char *msg;

    /* Process command line options. */
    bool optDaemonize = true;
    int c, optTimeout = 0;

    while( (c = getopt(argc,argv,"b:i:nh")) != -1 ) {
	switch( c ) {
	case 'b':
	    if( (optTimeout = atoi(optarg)) < 1 || optTimeout > 10000 )
	        usage(stderr);
	    break;
	case 'i':
	    if( !ConfigureIdleKey(optarg,&msg) ) {
	        fprintf(stderr,"gpiokey: %s\n",msg);
		return( 1 );
	    }
	    break;
	case 'n':
	    optDaemonize = false;
	    break;
	case 'h':
	    usage(stdout);
	    return( 0 );
	default:
	    usage(stderr);
	}
    }

    /* Build up list of GPIO ports to process. */
    while( optind < argc ) {
        int pin = atoi(argv[optind++]);

	/* Input is active-low logic? */
	bool neg;
	if( neg = (optind < argc && strcmp(argv[optind],"~") == 0) )
	    ++optind;

	/* Activate pull-up or pull-down resistor? */
	int res = 0;
	if( optind < argc && strcmp(argv[optind],"+") == 0 ) {
	    res = 1;
	    ++optind;
	}
	else if( optind < argc && strcmp(argv[optind],"-") == 0 ) {
	    res = -1;
	    ++optind;
	}

	if( optind >= argc ) {
	    fprintf(stderr,"gpiokey: key symbol expected for GPIO pin %d\n",pin);
	    return( 1 );
	}
	    
	/* Convert disambiguating keySymbols ++, --, and ~~ into their single
	   character equivalents. */
	const char *keyStr = argv[optind++];
	if( strcmp(keyStr,"++") == 0 || strcmp(keyStr,"--") == 0 || strcmp(keyStr,"~~") == 0 )
	    ++keyStr;

	if( !ConfigureInputPin(pin,neg,res,keyStr,&msg) ) {
	    fprintf(stderr,"gpiokey: GPIO error with pin %d: %s\n",pin,msg);
	    return( 1 );
	}
    }

    /* Do what it takes to become a daemon. */
    if( optDaemonize && daemon(0,0) != 0 ) {
	fprintf(stderr,"gpiokey: failed to become a daemon: %s\n",strerror(errno));
	return( 1 );
    }

    /* Ensure screen is in a known state initially. */
    system("vcgencmd display_power 1");
    bool screenOn = true;

    for( ;; ) {
	/* Scan GPIO inputs, generating key-down and key-up events. */
	ScanInputPins(screenOn);

	/* Blank screen after idle timeout. Unblank when no longer idle. */
	if( optTimeout != 0 ) {
	    int idle = IdleTime();
	    if( screenOn && idle >= optTimeout ) {
		system("vcgencmd display_power 0");
		screenOn = false;
	    }
	    else if( !screenOn && idle < optTimeout ) {
		system("vcgencmd display_power 1");
		screenOn = true;
	    }
	}

	/* Sleep for approximately 20ms between scans to minimize CPU usage and
	   provide debouncing, yet still give sufficiently fast response. */
	usleep(20000);
    }

    return( 0 );
}
