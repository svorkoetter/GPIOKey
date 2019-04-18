/* gpiokey - X Window System Wrappers

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
#include <stdbool.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/scrnsaver.h>
#include <X11/extensions/XTest.h>

#include "x11.h"

static Display *display = NULL;

static Display *openDisplay( void )
{
    if( display == NULL )
        display = XOpenDisplay(NULL);
    return( display );
}

KeyCode StringToKeyCode( const char *key, const char **err )
{
    KeySym ksym;
    if( (uint8_t) key[0] >= 0x20 && key[1] == '\0' )
        ksym = (KeySym) key[0];
    else {
	ksym = XStringToKeysym(key);
	if( ksym == NoSymbol ) {
	    *err = "unrecognized key symbol";
	    return( 0 );
	}
    }

    Display *disp = openDisplay();
    if( disp == NULL ) {
	*err = "failed to open display";
        return( 0 );
    }

    KeyCode code = XKeysymToKeycode(disp,ksym);
    if( code == 0 )
	*err = "key symbol has no corresponding key code";

    return( code );
}

void SendKeyCode( KeyCode keyCode, bool pressed )
{
    Display *disp = openDisplay();
    if( disp != NULL )
	XTestFakeKeyEvent(disp,keyCode,pressed,0);
}

/* Based on code found at https://superuser.com/questions/638357 */

/* Return user idle time in milliseconds. */
int IdleTime( void )
{
    Display *disp = openDisplay();
    if( disp == NULL )
        return( -1 );

    int event_base, error_base;
    if( !XScreenSaverQueryExtension(disp,&event_base,&error_base) )
	return( -2 );

    XScreenSaverInfo info;
    XScreenSaverQueryInfo(disp,DefaultRootWindow(disp),&info);
    return( info.idle );
}
