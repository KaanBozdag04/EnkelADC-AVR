#include <avr/io.h>
#include <avr/interrupt.h>
#include "LED_driver.h"

static volatile uint8_t *port;
static volatile uint8_t pin_mask;
static volatile uint16_t delay_ms = 500;
static volatile uint16_t tick_count = 0;

void gpio_init(uint8_t arduino_pin, uint16_t delay) {
    delay_ms = delay;

    if (arduino_pin <= 7) {
        DDRD |= (1 << arduino_pin);
        PORTD &= ~(1 << arduino_pin);
        port = &PORTD;
        pin_mask = (1 << arduino_pin);
    } else if (arduino_pin <= 13) {
        uint8_t bit = arduino_pin - 8;
        DDRB |= (1 << bit);
        PORTB &= ~(1 << bit);
        port = &PORTB;
        pin_mask = (1 << bit);
    } else {
        return; 
    }

    // Timer1 CTC mode – 1ms interrupt
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC, prescaler 64
    OCR1A = 249; // 1ms (16MHz/64/1000)
    TIMSK1 = (1 << OCIE1A);
    sei();
}

ISR(TIMER1_COMPA_vect) {
    tick_count++;
    if (tick_count >= delay_ms) {
        *port ^= pin_mask;
        tick_count = 0;
    }
}
