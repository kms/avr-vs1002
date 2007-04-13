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

int main(void) {
    /* Watchdog */
    wdt_reset();
    wdt_disable();

    /* System clock */
    //CLKPR = _BV(CLKPCE);
    //CLKPR = _BV(CLKPS3);

    /* Ports */
    PORTB = _BV(PINB2) | _BV(PINB3) | _BV(PINB4);
    DDRB = _BV(PINB0) | _BV(PINB1);

    /* Power saving */
    set_sleep_mode(SLEEP_MODE_IDLE);

    sei();

    for (;;) {
	sleep_mode();
    }
}
