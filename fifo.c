/* avr-vs1002
 * Streaming from serial port to VS1002.
 *
 * (c) Karl-Martin Skontorp <kms@skontorp.net> ~ http://picofarad.org/
 * Licensed under the GNU GPL 2.0 or later.
 */

#include <stdlib.h>
#include "fifo.h"

uint8_t isFifoEmpty(const fifo *c) {
    return c->unconsumed == 0;
}

uint8_t fifoSize(const fifo *c) {
    return c->unconsumed;
}

uint8_t fifoGet(fifo *c) {
    c->read = c->read & c->capacity;
    c->unconsumed--;
    return c->buffer[c->read++];
}       

void fifoPut(fifo *c, const uint8_t p) {
    if (c->unconsumed > c->capacity) {
        return;
    }
    c->write = c->write & c->capacity;
    c->buffer[c->write++] = p;
    c->unconsumed++;
}

fifo* fifoCreate(const uint8_t capacity) {
    fifo *f = malloc(sizeof(fifo) + (capacity * sizeof(uint8_t)));

    f->read = 0;
    f->write = 0;
    f->unconsumed = 0;
    f->capacity = capacity - 1;

    return f;
}   
