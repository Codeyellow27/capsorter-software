/*
 * Blink.c
 *
 * Created: 11/12/2019 4:18:45 PM
 * Author : Jonah
 */ 
#define F_CPU 16000000L
#define LED_STRIP_PORT PORTD
#define LED_STRIP_DDR  DDRD
#define LED_STRIP_PIN  0
#define p0 1
#define p1 (1 << 1)
#define p2 (1 << 2)
#define p3 (1 << 3)
#define p4 (1 << 4)
#define p5 (1 << 5)
#define p6 (1 << 6)
#define p7 (1 << 7)
#define DIV1 2
#define DIV2 40
#define USB_LED_OFF 0
#define USB_LED_ON  1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>

#include "usbdrv.h"
typedef struct rgb_color
{
	uint8_t red, green, blue;
} rgb_color;

int cpos = 0;
int spos = 0;
int div = 0;

void __attribute__((noinline)) led_strip_write(rgb_color * colors, uint16_t count)
{
	// Set the pin to be an output driving low.
	LED_STRIP_PORT &= ~(1<<LED_STRIP_PIN);
	LED_STRIP_DDR |= (1<<LED_STRIP_PIN);

	cli();   // Disable interrupts temporarily because we don't want our pulse timing to be messed up.
	while (count--)
	{
		// Send a color to the LED strip.
		// The assembly below also increments the 'colors' pointer,
		// it will be pointing to the next color at the end of this loop.
		asm volatile (
		"ld __tmp_reg__, %a0+\n"
		"ld __tmp_reg__, %a0\n"
		"rcall send_led_strip_byte%=\n"  // Send red component.
		"ld __tmp_reg__, -%a0\n"
		"rcall send_led_strip_byte%=\n"  // Send green component.
		"ld __tmp_reg__, %a0+\n"
		"ld __tmp_reg__, %a0+\n"
		"ld __tmp_reg__, %a0+\n"
		"rcall send_led_strip_byte%=\n"  // Send blue component.
		"rjmp led_strip_asm_end%=\n"     // Jump past the assembly subroutines.

		// send_led_strip_byte subroutine:  Sends a byte to the LED strip.
		"send_led_strip_byte%=:\n"
		"rcall send_led_strip_bit%=\n"  // Send most-significant bit (bit 7).
		"rcall send_led_strip_bit%=\n"
		"rcall send_led_strip_bit%=\n"
		"rcall send_led_strip_bit%=\n"
		"rcall send_led_strip_bit%=\n"
		"rcall send_led_strip_bit%=\n"
		"rcall send_led_strip_bit%=\n"
		"rcall send_led_strip_bit%=\n"  // Send least-significant bit (bit 0).
		"ret\n"

		// send_led_strip_bit subroutine:  Sends single bit to the LED strip by driving the data line
		// high for some time.  The amount of time the line is high depends on whether the bit is 0 or 1,
		// but this function always takes the same time (2 us).
		"send_led_strip_bit%=:\n"
		#if F_CPU == 8000000
		"rol __tmp_reg__\n"                      // Rotate left through carry.
		#endif
		"sbi %2, %3\n"                           // Drive the line high.

		#if F_CPU != 8000000
		"rol __tmp_reg__\n"                      // Rotate left through carry.
		#endif

		#if F_CPU == 16000000
		"nop\n" "nop\n"
		#elif F_CPU == 20000000
		"nop\n" "nop\n" "nop\n" "nop\n"
		#elif F_CPU != 8000000
		#error "Unsupported F_CPU"
		#endif

		"brcs .+2\n" "cbi %2, %3\n"              // If the bit to send is 0, drive the line low now.

		#if F_CPU == 8000000
		"nop\n" "nop\n"
		#elif F_CPU == 16000000
		"nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
		#elif F_CPU == 20000000
		"nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
		"nop\n" "nop\n"
		#endif

		"brcc .+2\n" "cbi %2, %3\n"              // If the bit to send is 1, drive the line low now.

		"ret\n"
		"led_strip_asm_end%=: "
		: "=b" (colors)
		: "0" (colors),         // %a0 points to the next color to display
		"I" (_SFR_IO_ADDR(LED_STRIP_PORT)),   // %2 is the port register (e.g. PORTC)
		"I" (LED_STRIP_PIN)     // %3 is the pin number (0-8)
		);

		// Uncomment the line below to temporarily enable interrupts between each color.
		//sei(); asm volatile("nop\n"); cli();
	}
	sei();          // Re-enable interrupts now that we are done.
	_delay_us(80);  // Send the reset signal.
}

#define LED_COUNT 4
rgb_color colors[LED_COUNT];
char sol = -1;

ISR (TIMER0_OVF_vect, ISR_NAKED) {
	sei();
	//wdt_reset();
	TCNT0 += 216; //every 2.5ms
	if (div%DIV1 == 0) {
		if (cpos == 0) PORTC = PORTC|p3|p0;//1001
		else if (cpos == 1) PORTC = PORTC|p1|p3;//1010
		else if (cpos == 2) PORTC = PORTC|p2|p1;//0110
		else if (cpos == 3) PORTC = PORTC|p0|p2;//0101
		cpos = (cpos+1)%4;
	} else {
		PORTC &= ~0xF;
	}
	if (div == DIV2) {
		if (sol > 0) sol--;
		else if (sol == 0) {
			sol--;
			PORTB &= ~3;
			PORTD &= ~0xF0;
		}
		if (spos == 0) PORTB = PORTB|p5|p2;//1001
		else if (spos == 1) PORTB = PORTB|p3|p5;//1010
		else if (spos == 2) PORTB = PORTB|p4|p3;//0110
		else if (spos == 3) PORTB = PORTB|p2|p4;//0101
		spos = (spos+1)%4;
		div = 0;
	} else if (div == 4) {
		PORTB &= ~0x3C;
		div++;
	} else div++;
	reti();
}

USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
    usbRequest_t *rq = (void *)data; // cast data to correct type
	usbMsgLen_t len = 64; 
    switch(rq->bRequest) { // custom command is in the bRequest field
    case 1:
        PORTB |= 1; // turn LED on
        break;
    case 2:
        PORTB |= 2; // turn LED off
        break;
    case 3:
        PORTD |= 0x10; // turn LED on
        break;
    case 4:
        PORTD |= 0x20; // turn LED off
        break;
    case 5:
        PORTD |= 0x40; // turn LED on
        break;
    case 6:
        PORTD |= 0x80; // turn LED off
        break;
    }
    sol = 1;
    len = rq->wLength.word;
    return 0; // should not get here
}

int main(void)
{
    DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD |= 0xF2;
	PORTC = 0;
	PORTB = 0;
	PORTD &= 0xF;
	int i=0;
	/*if ((MCUSR & 0x8) != 0) {
		i = 4;
	} else if ((MCUSR & 0x4) != 0) {
		i = 3;
	} else if ((MCUSR & 0x2) != 0) {
		i = 2;
	} else if ((MCUSR & 0x1) != 0) {
		i = 1;
	}*/
	for (i=i;i>0;i--) {
		PORTD |= 2;
		_delay_ms(500);
		PORTD &= ~2;
		_delay_ms(500);
	}
	MCUSR &= 0xF0;
	//_delay_ms(500);
	for (i = 0; i < LED_COUNT; i++) {
		colors[i] = (rgb_color){ 255, 255, 255};
	}
	led_strip_write(colors, LED_COUNT);
	_delay_ms(1000);
	PORTD |=2;
	//wdt_enable(WDTO_1S); // enable 1s watchdog timer
    usbDeviceDisconnect(); // enforce re-enumeration
    for(i = 0; i<250; i++) { // wait 500 ms
        //wdt_reset(); // keep the watchdog happy
        _delay_ms(2);
    }
    usbDeviceConnect();
    usbInit();
	sei();
	PORTD &= ~2;
	TCCR0 = (TCCR0&0xFC)|0x5;
	TIMSK |= (1 << TOIE0);
	
    while (1) 
    {
    	//wdt_reset(); // keep the watchdog happy
        usbPoll();
        _delay_ms(5);
    }
}

