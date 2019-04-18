TARGET = gpiokey
CC = gcc
CCFLAGS = -c -I$$HOME/include -std=c99 -O3 -Wall -Wno-parentheses -Wno-char-subscripts
LD = gcc
LIBS = -lm -lX11 -lXtst

$(TARGET): gpio.o main.o x11.o
	$(LD) $(LDFLAGS) -o $(TARGET) *.o $(LIBS)
	strip $(TARGET)

gpio.o: gpio.c x11.h
	$(CC) $(CCFLAGS) gpio.c

main.o: main.c gpio.h x11.h
	$(CC) $(CCFLAGS) main.c

x11.o: x11.c x11.h
	$(CC) $(CCFLAGS) x11.c

clean:
	rm -f gpio.o
	rm -f main.o
	rm -f x11.o
