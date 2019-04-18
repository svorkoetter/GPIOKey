# gpiokey - A GPIO-to-Keypress and Display Power Management Daemon for Raspberry Pi

The `gpiokey` utility is a daemon for use with Raspbian, intended to be
launched from your X11 session manager's auto-start file. It performs two
separate yet somewhat related functions:

* Monitors a specified set of GPIO pins, converting state changes to key-down
  and key-up events.
* Monitors X11 idle time (no input from the keyboard, mouse, or monitored GPIO
  pins), and turns off the display when idle.

## System Requirements

The following libraries and utilities are used by `gpiokey` (some of these
might already be installed by default):

* libx11-6
* libxss1
* libxtst6
* libraspberrypi-bin for the `vcgencmd` utility
* WiringPi for the `gpio` utility

If you want to rebuild `gpiokey` from source, you will also need the following:

* libx11-dev
* libxss-dev
* libxtst-dev

## Usage

The `gpiokey` command should be launched from
`~/.config/lxsession/LXDE-pi/autostart`, or the equivalent if you're using a
different window manager than LXDE. In any case, it shouldn't be launched until
after your X11 session is up and running.

The command line syntax for `gpiokey` is as follows:

    gpiokey [-b minutes] [-i keySym] [-n] [-h] { gpioPin [~] [+|-] keySym }

In the above, `[]` indicates optional items, `{}` indicates items that can be
repeated zero or more times, and `|` means "either/or".

If the `-b` option is specified, followed by a positive integer between 1 and
10000, the screen will be blanked (powered down if supported by the monitor)
after the specified number of minutes of idle time. Any keyboard, mouse, or
GPIO input (as specified by later arguments) will reactivate the screen.

When a GPIO input becomes active while the system is idle and the screen is
blank, the associated key event is sent, causing the screen to be reactivated,
and causing the foreground application to respond to the key. The `-i` option
can be used to specify an alternate key to send when in the idle state. For
example, sending a left shift key (`Shift_L`) will wake up the screen without
affecting the running application.

By default, `gpiokey` runs as a daemon. The invocation of the `gpiokey` command
will return immediately, but the daemon will continue to run in the background.
If the `-n` option is given, then `gpiokey` will remain in the foreground until
you terminate it by pressing `Ctrl+C`.

The `-h` option displays a summary of the command line syntax.

### GPIO Pin to Key Mapping

The remaining arguments specify zero or more GPIO pins to monitor for input
and transform into keyboard events (whatever application is running in the
foreground will think you have pressed the corresponding key on the keyboard).

Each pin is specified by two to four arguments as follows:

* The _gpioPin_ refers to the BCM GPIO pin number (not the non-standard
  WiringPi pin number, nor the physical pin number of the connector). Valid
  values are 0 to 31, although which pins are actually available depend on the
  Raspberry Pi model being used.

* The optional character `~`, if present, indicates that active-low logic is
  being used for this input (e.g., pressing a button produces a low input). If
  absent, active-high logic is assumed.

* An optional `+` or `-` character, if present, indicates that the GPIO pin's
  internal pull-up or pull-down resistor respectively should be activated.

* Finally, the _keySym_ specifies the key to press when the corresponding
  input becomes active (and release when it becomes inactive). This can be
  either a single printable character, or a word describing a special key. The
  latter correspond to the `XK_` definitions in
  `/usr/include/X11/keysymdefs.h`, with the `XK_` prefix omitted. For example,
  `Right` refers to the right-arrow key, and `Shift_L` refers to the left shift
  key. To specify `+`, `-`, or `~` as the _keySym_, type the character twice
  so it is not misinterpreted as one of the optional arguments above (this is
  not necessary if both optional arguments are actually present).

### Sample GPIO Setup

Consider a configuration with two normally open pushbuttons, one connected
between GPIO 23 (physical pin 16) and ground (such as physical pin 20), and the
other between GPIO 25 (physical pin 22) and ground. Pressing a button thus
sends a low signal to the respective pin, so this is active-low logic. To
ensure the pin is high when a button is not pressed, the internal pull-up
resistors are required.

The following invocation of `gpiokey` supports the above scenario, with a 20
minute screen blanking timeout, and sends a left shift key if one of the
buttons is pressed while the screen is blank:

    gpiokey -b 20 -i Shift_L 23 ~ + Left 25 ~ + Right

More buttons corresponding to more keys can easily be added (up to 28 depending
on the model of Raspberry Pi).

## Internals

Port setup is done by `gpiokey` using WiringPi's `gpio` utility. This was done
to avoid the need to run `gpiokey` with root privileges. If you are using a
Raspberry Pi 3 or newer, you may need to manually install a compatible version
of WiringPi from <http://wiringpi.com/download-and-install>.

Idle time determination is done using the `XScreenSaverQueryInfo` function of
the `libxss` library.
 
Screen blanking and unblanking is done using the `vcgencmd` utility, invoked as
`vcgencmd display_power x`, where _x_ is 0 or 1 respectively.
