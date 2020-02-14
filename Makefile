# WinAVR cross-compiler toolchain is used here
CC = avr-gcc
OBJCOPY = avr-objcopy
DUDE = avrdude

# If you are not using ATtiny2313 and the USBtiny programmer, 
# update the lines below to match your configuration
CFLAGS = -Wall -Os -Iusbdrv -mmcu=atmega8
OBJFLAGS = -j .text -j .data -O ihex
DUDEFLAGS = -p atmega8 -c usbasp -v

# Object files for the firmware (usbdrv/oddebug.o not strictly needed I think)
OBJECTS = usbdrv/usbdrv.o usbdrv/oddebug.o usbdrv/usbdrvasm.o main.o

# Command-line client
CMDLINE = capsorter.exe

# By default, build the firmware and command-line client, but do not flash
all: main.hex $(CMDLINE)

ml : ml.c
	gcc -o $@ -L "C:/Users/jmegg/Downloads/opencv/build/x86/vc14/lib" -I "C:/Users/jmegg/Downloads/opencv/build/include" ml.c -lopencv_core2413d -lopencv_imgproc2413d -lopencv_highgui2413d -lopencv_ml2413d -lopencv_video2413d -lopencv_features2d2413d -lopencv_calib3d2413d -lopencv_objdetect2413d -lopencv_contrib2413d -lopencv_legacy2413d -lopencv_flann2413d


# With this, you can flash the firmware by just typing "make flash" on command-line
flash: main.hex
	$(DUDE) $(DUDEFLAGS) -U flash:w:$<

# One-liner to compile the command-line client from usbtest.c
$(CMDLINE): capsorter.c
	gcc -I ./libusb/libusb-win32-bin-1.2.6.0/include -L ./libusb/libusb-win32-bin-1.2.6.0/lib/gcc -O capsorter.c -o capsorter.exe -lusb -lgdi32

# Housekeeping if you want it
clean:
	$(RM) *.o *.hex *.elf usbdrv/*.o

# From .elf file to .hex
%.hex: %.elf
	$(OBJCOPY) $(OBJFLAGS) $< $@

# Main.elf requires additional objects to the firmware, not just main.o
main.elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

# Without this dependance, .o files will not be recompiled if you change 
# the config! I spent a few hours debugging because of this...
$(OBJECTS): usbdrv/usbconfig.h

# From C source to .o object file
%.o: %.c	
	$(CC) $(CFLAGS) -c $< -o $@

# From assembler source to .o object file
%.o: %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

usbtest: usbtest.c
	gcc -I ./libusb/libusb-win32-bin-1.2.6.0/include -L ./libusb/libusb-win32-bin-1.2.6.0/lib/gcc -O usbtest.c -o usbtest.exe -lusb -lgdi32