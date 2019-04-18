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

#ifndef __GPIOKEY_GPIO_H__
#define __GPIOKEY_GPIO_H__

/* Configures the specified GPIO pin (BCM numbering) as an input, optionally
   with negative logic (a low input is considered active), and optionally with
   the internal pull-up (pud > 0) or pull-down (pud < 0) resistor enabled. The
   key should be a single-character string, or a word corresponding to one of
   the XK_ definitions in the /usr/include/X11/keysymdef.h file. Returns false
   and sets *err to point to an error message on failure. */
extern bool ConfigureInputPin( int pin, bool neg, int pud, const char *key,
			       const char **err );

/* Sets the key to be sent if a configured GPIO input becomes active while the
   system is idle (screen is blank). */
extern bool ConfigureIdleKey( const char *key, const char **err );

/* Scans all configured GPIO pins, producing a key-down event when one becomes
   active, and a key-up event when it becomes inactive. */
extern void ScanInputPins( bool screenOn );

#endif
