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

#ifndef __GPIOKEY_X11_H__
#define __GPIOKEY_X11_H__

#include <X11/Xlib.h>

/* Converts a string containing a single character (e.g. "A"), or a word
   matching one of the key symbol names in /usr/include/X11/keysymdef.h
   (e.g. "Shift_L"), to an X11 key code. If the conversion fails, returns 0
   and sets *err to point to an error message. */
extern KeyCode StringToKeyCode( const char *key, const char **err );

/* Send a key-down or key-up event for the specified key code. */
extern void SendKeyCode( KeyCode keyCode, bool down );

#endif
