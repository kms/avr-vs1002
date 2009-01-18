/* avr-vs1002
 * Streaming from serial port to VS1002.
 *
 * (c) Karl-Martin Skontorp <kms@skontorp.net> ~ http://picofarad.org/
 * Licensed under the GNU GPL 2.0 or later.
 */

#define F_CPU 18.432e6

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "fifo.h"

/* PD5: RST */
/* PD6: DREQ */
/* PD7: XDCS */
/* PB0: XCS */
/* PB2: SS */
/* PB3: MOSI */
/* PB4: MISO */
/* PB5: SCK */
#define VS1002_RST_PORT PORTD
#define VS1002_RST_PIN PIND5
#define VS1002_DREQ_PORT PORTD
#define VS1002_DREQ_PIN PIND6
#define VS1002_XCS_PORT PORTB
#define VS1002_XCS_PIN PINB0
#define VS1002_XDCS_PORT PORTD
#define VS1002_XDCS_PIN PIND7
#define VS1002_SI_PORT PORTB
#define VS1002_SI_PIN PINB3
#define VS1002_SO_PORT PORTB
#define VS1002_SO_PIN PINB4
#define VS1002_SCK_PORT PORTB
#define VS1002_SCK_PIN PINB5

void vs1002cmd_s(void);
void vs1002cmd_e(void);
void vs1002data_s(void);
void vs1002data_e(void);
void vs1002Reset(void);
void uartTx(char a);

//fifo *b;

void uartTx(const char a) {
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = a;
}

/*ISR(USART_RX_vect) {
  while (UCSR0A & _BV(RXC0)) {
  fifoPut(b, UDR0);
  }    
  } 
  */

void vs1002cmd_s() {
    VS1002_XCS_PORT &= ~(_BV(VS1002_XCS_PIN));
    VS1002_XDCS_PORT |= _BV(VS1002_XDCS_PIN);
}

void vs1002cmd_e() {
    VS1002_XCS_PORT |= _BV(VS1002_XCS_PIN);
    VS1002_XDCS_PORT &= ~(_BV(VS1002_XDCS_PIN));
}

void vs1002data_s() {
    VS1002_XCS_PORT |= _BV(VS1002_XCS_PIN);
    VS1002_XDCS_PORT &= ~(_BV(VS1002_XDCS_PIN));
    _delay_us(1);
}

void vs1002data_e() {
    VS1002_XCS_PORT &= ~(_BV(VS1002_XCS_PIN));
    VS1002_XDCS_PORT |= _BV(VS1002_XDCS_PIN);
    _delay_us(1);
}

void vs1002Reset() {
    VS1002_RST_PORT &= ~(_BV(VS1002_RST_PIN));
    _delay_ms(5);
    VS1002_RST_PORT |= _BV(VS1002_RST_PIN);
    _delay_ms(5);
}

int main(void) {
    /* Watchdog */
    wdt_reset();
    wdt_disable();

    /* Ports */
    DDRB = _BV(PINB0) | _BV(PINB2) | _BV(PINB3) | _BV(PINB5);
    DDRD = _BV(PIND5) | _BV(PIND7);
    PORTB = 0xff; //& (_BV(PINB0));
    PORTC = 0xff;
    PORTD = 0xff;// & (_BV(PIND5) | _BV(PIND6) | _BV(PIND7));

    /* Power saving */
    set_sleep_mode(SLEEP_MODE_IDLE);

    //b = fifoCreate(255);
    vs1002Reset();

    DDRD |= _BV(DDD1);
    UCSR0B = _BV(TXEN0) | _BV(RXEN0); // | _BV(RXCIE0);
    UBRR0L = 9;

    /* SPI */
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0);
    //SPSR = _BV(SPI2X);

    // 0x8000 + 6144 = 0x9800, 12.288MHz XTAL + clk-doubling

    /* Clock */
    vs1002cmd_s();
    SPDR = 0x02;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x03;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x98;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    vs1002cmd_e();

    /* Mode: TEST */
    vs1002cmd_s();
    SPDR = 0x02;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x08;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x20; // Test
    loop_until_bit_is_set(SPSR, SPIF);
    vs1002cmd_e();

    /* Volume */
    vs1002cmd_s();
    SPDR = 0x02;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x0b;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    vs1002cmd_e();

    /* Test 1.500kHz sine */
    vs1002data_s();
    SPDR = 0x53;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0xef;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x6e;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x28;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    vs1002data_e();

    uint8_t t;

    for (t = 0; t < 125; t++) {
	_delay_ms(2);
    }

    /* End test */
    vs1002data_s();
    SPDR = 0x45;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x78;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x69;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x74;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    vs1002data_e();

    /* Mode: PLAY */
    vs1002cmd_s();
    SPDR = 0x02;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x08;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x08; // STREAM
    loop_until_bit_is_set(SPSR, SPIF);
    vs1002cmd_e();

    uint16_t i;
    uint8_t buffer[1000];

    for (;;) {
	uartTx('.');

	for (i = 0; i < 1000; i++) {
	    loop_until_bit_is_set(UCSR0A, RXC0);
	    buffer[i] = UDR0;
	}

	loop_until_bit_is_set(VS1002_DREQ_PORT, VS1002_DREQ_PIN);

	vs1002data_s();
	for (i = 0; i < 1000; i++) {
	    loop_until_bit_is_set(SPSR, SPIF);
	    SPDR = buffer[i];
	}
	vs1002data_e();
    }
}
