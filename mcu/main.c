#define  INCLUDE_FROM_BULKVENDOR_C
#ifndef _BULK_VENDOR_H_
#define _BULK_VENDOR_H_


// General pin definitions
#define p0 1
#define p1 (1 << 1)
#define p2 (1 << 2)
#define p3 (1 << 3)
#define p4 (1 << 4)
#define p5 (1 << 5)
#define p6 (1 << 6)
#define p7 (1 << 7)

/* Length and Period of belt, not used, 
   mostly just a way of me remembering them. 
   Note these have changed because the 
   belt has been extended, but the velocity 
   is the same */
#define LENGTH 5760 //mm
#define PERIOD 6660 //ms

/* TIMING: the microcontroller receives the solenoid 
   number and the distance delay thus far in mm/OFFSET_SCALING_FACTOR 
   (to allow a larger range of distances to be sent). 
   This delay is rescaled to mm, and subtracted from 
   SOLn_DIST to calculate the distance the cap has 
   yet to travel. This is converted to time (ms) 
   by multiplying by VELOCITY_INV, a GENERIC_CAP_DELAY 
   and SOLn_TRIM are applied and this value is added to curr_time.

*/
#define MAX_TIMER_VAL 250
#define TIMER_OFFSET 247
#define SOLENOID_ON_DURATION 50
#define GENERIC_CAP_DELAY 85
#define OFFSET_SCALING_FACTOR 2
#define SOL1_DIST 202
#define SOL2_DIST 302
#define SOL3_DIST 402
#define SOL4_DIST 502
#define SOL5_DIST 602
#define SOL6_DIST 702
#define SOL7_DIST 802
#define SOL8_DIST 902
#define SOL9_DIST 1002
#define SOL10_DIST 1102
#define SOL11_DIST 1202
#define SOL12_DIST 1302
#define SOL13_DIST 1402
#define SOL14_DIST 1502
#define SOL15_DIST 1602
#define SOL16_DIST 1702
#define SOL17_DIST 1802
#define SOL18_DIST 1902
#define SOL19_DIST 2002
#define SOL20_DIST 2102
#define SOL1_TRIM 3
#define SOL2_TRIM 0
#define SOL3_TRIM -15
#define SOL4_TRIM -20
#define SOL5_TRIM -33
#define SOL6_TRIM -67
#define SOL7_TRIM -72
#define SOL8_TRIM 0
#define SOL9_TRIM 0
#define SOL10_TRIM 0
#define SOL11_TRIM 0
#define SOL12_TRIM 0
#define SOL13_TRIM 0
#define SOL14_TRIM 0
#define SOL15_TRIM 0
#define SOL16_TRIM 0
#define SOL17_TRIM 0
#define SOL18_TRIM 0
#define SOL19_TRIM 0
#define SOL20_TRIM 0
#define VELOCITY_INV 1.15625
//#define LENGTH/PERIOD LENGTH/PERIOD //m/s

/* Includes: */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "PQ.h"


#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <lufa-LUFA-170418/LUFA/Drivers/USB/USB.h>

		#include <avr/pgmspace.h>

	/* Macros: */
		/** Endpoint address of the Bulk Vendor device-to-host data IN endpoint. */
		#define VENDOR_IN_EPADDR               (ENDPOINT_DIR_IN  | 1)

		/** Endpoint address of the Bulk Vendor host-to-device data OUT endpoint. */
		#define VENDOR_OUT_EPADDR              (ENDPOINT_DIR_OUT | 2)

		/** Size in bytes of the Bulk Vendor data endpoints. */
		#define VENDOR_IO_EPSIZE               64

	/* Type Defines: */
		/** Type define for the device configuration descriptor structure. This must be defined in the
		 *  application code, as the configuration descriptor contains several sub-descriptors which
		 *  vary between devices, and which describe the device's usage to the host.
		 */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;

			// Vendor Interface
			USB_Descriptor_Interface_t            Vendor_Interface;
			USB_Descriptor_Endpoint_t             Vendor_DataInEndpoint;
			USB_Descriptor_Endpoint_t             Vendor_DataOutEndpoint;
		} USB_Descriptor_Configuration_t;

		/** Enum for the device interface descriptor IDs within the device. Each interface descriptor
		 *  should have a unique ID index associated with it, which can be used to refer to the
		 *  interface from other descriptors.
		 */
		enum InterfaceDescriptors_t
		{
			INTERFACE_ID_Vendor = 0, /**< Vendor interface descriptor ID */
		};

		/** Enum for the device string descriptor IDs within the device. Each string descriptor should
		 *  have a unique ID index associated with it, which can be used to refer to the string from
		 *  other descriptors.
		 */
		enum StringDescriptors_t
		{
			STRING_ID_Language     = 0, /**< Supported Languages string descriptor ID (must be zero) */
			STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
			STRING_ID_Product      = 2, /**< Product string ID */
		};

	/* Function Prototypes: */
		uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
		                                    const uint16_t wIndex,
		                                    const void** const DescriptorAddress)
		                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification       = VERSION_BCD(1,1,0),
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = 0x16C0,
	.ProductID              = 0x05DC,
	.ReleaseNumber          = VERSION_BCD(0,0,1),

	.ManufacturerStrIndex   = STRING_ID_Manufacturer,
	.ProductStrIndex        = STRING_ID_Product,
	.SerialNumStrIndex      = USE_INTERNAL_SERIAL,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	.Config =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

			.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
			.TotalInterfaces        = 1,

			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,

			.ConfigAttributes       = USB_CONFIG_ATTR_RESERVED,

			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
		},

	.Vendor_Interface =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = INTERFACE_ID_Vendor,
			.AlternateSetting       = 0,

			.TotalEndpoints         = 2,

			.Class                  = 0xFF,
			.SubClass               = 0xFF,
			.Protocol               = 0xFF,

			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.Vendor_DataInEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = VENDOR_IN_EPADDR,
			.Attributes             = (EP_TYPE_CONTROL | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = VENDOR_IO_EPSIZE,
			.PollingIntervalMS      = 0x05
		},

	.Vendor_DataOutEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = VENDOR_OUT_EPADDR,
			.Attributes             = (EP_TYPE_CONTROL | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = VENDOR_IO_EPSIZE,
			.PollingIntervalMS      = 0x05
		}
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"Jonah Meggs");

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"CapSorter");

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = &DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case STRING_ID_Language:
					Address = &LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case STRING_ID_Manufacturer:
					Address = &ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case STRING_ID_Product:
					Address = &ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
			}

			break;
	}

	*DescriptorAddress = Address;
	return Size;
}

#include <lufa-LUFA-170418/LUFA/Drivers/USB/USB.h>
#include <lufa-LUFA-170418/LUFA/Platform/Platform.h>

/* Function Prototypes: */
void SetupHardware(void);

void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

#endif


/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */


PQ q;
char curr_time;

ISR (TIMER0_OVF_vect) {
	TCNT0 += TIMER_OFFSET; //1ms for 247 //every 2.5ms for 236
	ItemPQ item;
	if (!PQIsEmpty(q)) {
		item = PQPeek(q);
		while (item.value <= curr_time) {
			PQDequeue(q);
			if (item.key > 0) {
				PORTD |= 1 << (item.key-1);
				item.key = 0-item.key;
				item.value = curr_time+SOLENOID_ON_DURATION;
				PQAdd(q, item);
			} else {
				PORTD &= ~(1 << (-1-item.key));
			}
			if (PQIsEmpty(q)) break;
			item = PQPeek(q);
		}
	}
	curr_time += 1;
	if (curr_time >= MAX_TIMER_VAL) {
		curr_time -= MAX_TIMER_VAL;
		PQShiftPriority(q, MAX_TIMER_VAL);
	}
}

int main(void)
{
	//_delay_ms(2000);
    DDRB = 0xFF;
    DDRD = 0xFF;
    PORTD = 0;
    PORTD |= 0x80;
    _delay_ms(500);
    PORTD &= ~0x80;
    _delay_ms(500);
    PORTB = 0;
	q = PQNew();
	PORTD |= 0x80;
    _delay_ms(100);
    PORTD &= ~0x80;
    _delay_ms(100);

	curr_time = 0;
	sei();
	TCCR0B |= 5;
	TIMSK0 = (1 << TOIE0);
	SetupHardware();

	
	GlobalInterruptEnable();

	for (;;)
	{
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	//clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
	/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
	XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
	XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

	/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
	XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
	XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

	/* Hardware Initialization */
	//LEDs_Init();
	USB_Init();
}


/** Event handler for the USB_ConfigurationChanged event. This is fired when the host set the current configuration
 *  of the USB device after enumeration - the device endpoints are configured.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup Vendor Data Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR_IN_EPADDR,  EP_TYPE_CONTROL, VENDOR_IO_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR_OUT_EPADDR, EP_TYPE_CONTROL, VENDOR_IO_EPSIZE, 1);

	/* Indicate endpoint configuration success or failure */
	//LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
	char offset;
	if (USB_ControlRequest.bmRequestType != 0x40) return;
	ItemPQ item;
	item.key = USB_ControlRequest.bRequest;
	Endpoint_ClearSETUP();
	Endpoint_Read_Control_Stream_LE(&offset, 1);
	Endpoint_ClearIN();
	switch(item.key) { // custom command is in the bRequest field
		case 1:
			item.value = curr_time+VELOCITY_INV*(SOL1_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL1_TRIM;
			break;
		case 2:
			item.value = curr_time+VELOCITY_INV*(SOL2_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL2_TRIM;
			break;
		case 3:
			item.value = curr_time+VELOCITY_INV*(SOL3_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL3_TRIM;
			break;
		case 4:
			item.value = curr_time+VELOCITY_INV*(SOL4_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL4_TRIM;
			break;
		case 5:
			item.value = curr_time+VELOCITY_INV*(SOL5_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL5_TRIM;
			break;
		case 6:
			item.value = curr_time+VELOCITY_INV*(SOL6_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL6_TRIM;
			break;
		case 7:
			item.value = curr_time+VELOCITY_INV*(SOL7_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL7_TRIM;
			break;
		case 8:
			item.value = curr_time+VELOCITY_INV*(SOL8_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL8_TRIM;
			break;
		case 9:
			item.value = curr_time+VELOCITY_INV*(SOL9_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL9_TRIM;
			break;
		case 10:
			item.value = curr_time+VELOCITY_INV*(SOL10_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL10_TRIM;
			break;
		case 11:
			item.value = curr_time+VELOCITY_INV*(SOL11_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL11_TRIM;
			break;
		case 12:
			item.value = curr_time+VELOCITY_INV*(SOL12_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL12_TRIM;
			break;
		case 13:
			item.value = curr_time+VELOCITY_INV*(SOL13_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL13_TRIM;
			break;
		case 14:
			item.value = curr_time+VELOCITY_INV*(SOL14_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL14_TRIM;
			break;
		case 15:
			item.value = curr_time+VELOCITY_INV*(SOL15_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL15_TRIM;
			break;
		case 16:
			item.value = curr_time+VELOCITY_INV*(SOL16_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL16_TRIM;
			break;
		case 17:
			item.value = curr_time+VELOCITY_INV*(SOL17_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL17_TRIM;
			break;
		case 18:
			item.value = curr_time+VELOCITY_INV*(SOL18_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL18_TRIM;
			break;
		case 19:
			item.value = curr_time+VELOCITY_INV*(SOL19_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL19_TRIM;
			break;
		case 20:
			item.value = curr_time+VELOCITY_INV*(SOL20_DIST-offset*mul)-GENERIC_CAP_DELAY+SOL20_TRIM;
			break;
		}
	if (item.value < curr_time) item.value = curr_time;
	if (item.key <= 7 && item.key > 0) {
		PQAdd(q, item);
		PORTD ^= 0x80;
	}
}
