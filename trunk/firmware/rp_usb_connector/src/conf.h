#ifndef _RP_BRIDGE_CONF_H
#define _RP_BRIDGE_CONF_H


//-------------------------------------------------
// GENERAL FEATURES

//#define _DEBUG
#define _ENABLE_AVRDOPER

//-------------------------------------------------

// Jumper used for mode selection
#define JUMPER_MODE_SEL         16   //PC2

#define PORT_LED        PORTD
#define DDR_LED         DDRD
#define LED_TX          PD7
#define LED_RX          PD6

#define LED_INIT()               \
    do{                          \
        cbi(PORT_LED, LED_TX);   \
        cbi(PORT_LED, LED_RX);   \
        cbi(DDR_LED, LED_TX);    \
        cbi(DDR_LED, LED_RX);    \
    }while(0)

#define LED_RX_ON()    sbi(DDR_LED, LED_RX)
#define LED_TX_ON()    sbi(DDR_LED, LED_TX)

#define LED_RX_OFF()    cbi(DDR_LED, LED_RX)
#define LED_TX_OFF()    cbi(DDR_LED, LED_TX)

//mapping according to The Sunjar++ Main Circuit 1.0 Alpha (Rev 1)

// ArduinoLite Pin mapping:
// ATMEL ATMEGA8 & 168 
//
//                  +-\/-+
//            PC6  1|    |28  PC5 (AI 5/*D19)
//      (D 0) PD0  2|    |27  PC4 (AI 4/*D18)
//      (D 1) PD1  3|    |26  PC3 (AI 3/*D17)
//      (D 2) PD2  4|    |25  PC2 (AI 2/*D16)
// PWM+ (D 3) PD3  5|    |24  PC1 (AI 1/*D15)
//      (D 4) PD4  6|    |23  PC0 (AI 0/*D14)
//            VCC  7|    |22  GND
//            GND  8|    |21  AREF
//     *(D20) PB6  9|    |20  AVCC
//     *(D21) PB7 10|    |19  PB5 (D 13)
// PWM+ (D 5) PD5 11|    |18  PB4 (D 12)
// PWM+ (D 6) PD6 12|    |17  PB3 (D 11) PWM
//      (D 7) PD7 13|    |16  PB2 (D 10) PWM
//      (D 8) PB0 14|    |15  PB1 (D 9) PWM
//                  +----+



//-------------------------------------------------
// AVRDOPER-LITE
/*
#define	ISP_OUT   PORTB
#define ISP_IN    PINB
#define ISP_DDR   DDRB
#define ISP_RST   PB2
#define ISP_MOSI  PB3
#define ISP_MISO  PB4
#define ISP_SCK   PB5
*/

// represented in ArduinoLite Pin
#define ISP_RST   10
#define ISP_MOSI  11
#define ISP_MISO  12
#define ISP_SCK   13

#define USB_PORT_NAME           B  //USB Port 
#define USB_PORT                PORTB
#define USB_DDR                 DDRB
#define USB_PORT_P              1  //USB D+ Pin number
#define USB_PORT_N              0  //USB D- Pin number

// The following marcos are not used yet.
// The avr-usb always uses Int0 
#define ON_USB_INT              0
#define ON_USB_VECTOR           INT0_vect


/* configuration options: */
#define USE_DCD_REPORTING       0
/* If this option is defined to 1, the driver will report carrier detect when
 * the serial device is opened. This is useful on some Unix platforms to allow
 * using the /dev/tty* instead of /dev/cu* devices.
 * Setting this option to 0 saves a couple of bytes in flash and RAM memory.
 */



#define ENABLE_DEBUG_INTERFACE  0
/* If this option is defined to 1, the device buffers serial data (read from
 * the the ISP connector) and makes it available through vendor specific
 * requests to host based software. This is useful to display debug information
 * sent by the target.
 * Setting this option to 0 saves a couple of bytes in flash memory.
 */


#define ENABLE_HID_INTERFACE    1
/* If this option is defined to 1, the device implements a custom HID type
 * interface to send and receive STK500 serial data. If both, the CDC-ACM and
 * HID interface are enabled, a jumper selects which one is used.
 */


#define ENABLE_CDC_INTERFACE    0
/* If this option is defined to 1, the device implements a CDC-ACM modem,
 * emulating the behavior of the STK500 board on a virtual COM port.
 */


#define ENABLE_HVPROG           0
/* If this option is defined to 1, the high voltage programmer is enabled.
 */


#define HW_CDC_PACKET_SIZE      8
/* Size of bulk transfer packets. The standard demands 8 bytes, but we may
 * be better off with less. Try smaller values if the communication hangs.
 */


#define HW_DEBUG_BAUDRATE       19200
/* Bit-rate for the UART (for reading debug data from the target).
 */


//-------------------------------------------------
// GENERAL IO BOARD

// ADC & PWM  (ArduinoLite Pins)
#define IO_PWM0    11//PB3
#define IO_PWM1    12//PB4  

#define IO_ADC0    15
#define IO_ADC1    17


#define A_ADC0   1
#define A_ADC1   3
#define A_ADC2   6
#define A_ADC3   7
#endif