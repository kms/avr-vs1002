/* avr-vs1002
 * Streaming from serial port to VS1002.
 *
 * (c) Karl-Martin Skontorp <kms@skontorp.net> ~ http://22pf.org/
 * Licensed under the GNU GPL 2.0 or later.
 */

#ifndef _FIFO_H
#define _FIFO_H 1

#include <stdint.h>

typedef struct {
    volatile uint8_t read;
    volatile uint8_t write;
    volatile uint8_t unconsumed;
    uint8_t capacity;
    uint8_t buffer[0];
} fifo;

uint8_t isFifoEmpty(const fifo *c);
uint8_t fifoSize(const fifo *c);
uint8_t fifoGet(fifo *c);
void fifoPut(fifo *c, const uint8_t p);
fifo* fifoCreate(const uint8_t capacity);
#endif
