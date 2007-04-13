/*
 * $Id$
 * Karl-Martin Skontorp <kms@skontorp.net>
 */

#define F_CPU 18e6

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

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

void vs1002cmd_s() {
    VS1002_XCS_PORT &= ~(_BV(VS1002_XCS_PIN));
    VS1002_XDCS_PORT |= _BV(VS1002_XDCS_PIN);
    _delay_us(1);
}

void vs1002cmd_e() {
    VS1002_XCS_PORT |= _BV(VS1002_XCS_PIN);
    VS1002_XDCS_PORT &= ~(_BV(VS1002_XDCS_PIN));
    _delay_us(1);
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
    PORTB = 0xff & (_BV(PINB0));
    PORTC = 0xff;
    PORTD = 0xff & (_BV(PIND5) | _BV(PIND6) | _BV(PIND7));

    /* Power saving */
    set_sleep_mode(SLEEP_MODE_IDLE);

    vs1002Reset();

    /* SPI */
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);

    // 0x8000 + 6144 = 0x9800, 12.288MHz XTAL + clk-doubling

    /* Clock */
    vs1002cmd_s();
    SPDR = 0x02;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x03;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x98;
    loop_until_bit_is_set(SPSR, SPIF);
    vs1002cmd_e();

    /* Mode */
    vs1002cmd_s();
    SPDR = 0x02;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x00;
    loop_until_bit_is_set(SPSR, SPIF);
    SPDR = 0x08;
    loop_until_bit_is_set(SPSR, SPIF);
    //SPDR = 0x20; // Test
    SPDR = 0x00;
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

    /* Test */
    //vs1002data_s();
    //SPDR = 0x53;
    //loop_until_bit_is_set(SPSR, SPIF);
    //SPDR = 0xef;
    //loop_until_bit_is_set(SPSR, SPIF);
    //SPDR = 0x6e;
    //loop_until_bit_is_set(SPSR, SPIF);
    //SPDR = 0x7e;
    //loop_until_bit_is_set(SPSR, SPIF);
    //SPDR = 0x00;
    //loop_until_bit_is_set(SPSR, SPIF);
    //SPDR = 0x00;
    //loop_until_bit_is_set(SPSR, SPIF);
    //SPDR = 0x00;
    //loop_until_bit_is_set(SPSR, SPIF);
    //SPDR = 0x00;
    //loop_until_bit_is_set(SPSR, SPIF);
    //vs1002data_e();

    //sei();

    for (;;) {
	loop_until_bit_is_set(VS1002_DREQ_PORT, VS1002_DREQ_PIN);
    }
}
