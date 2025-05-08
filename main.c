#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

//############################################
//#                UART                      #
//############################################

// Se kapitel 19 i dokumentationen
void uart_init(unsigned int ubrr) {
    // UBRR0 = USART Baud Rate Register 0, 
    // Ställ in baudrate, 16-bit så vi har en high 8 bit + en low 8 bit
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;

    // Aktivera sändning och mottagning
    UCSR0B = (1 << TXEN0); // Endast sändning här
    // UCSR0B = (1 << RXEN0); // Om vi skulle vilja aktivera motagning 

    // UCSR0C – Control Register C
    // Se Table 19-7. UCSZn Bits Settings för olika konfigureringar
    // Välj asynkront läge, 8 databitar, 1 stoppbit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(unsigned char data) {
    // Vänta tills sändbufferten är tom
    // UCSR0A – USART Control and Status Register A
    // UDRE0 – USART Data Register Empty
    while (!(UCSR0A & (1 << UDRE0))); // Vänta tills sändbuffert är tom genom att jämför hur ett tomt register ska se ut

    // UDR0 – USART Data Register, det är här vi kan hämta eller ladda data för att skicka!
    // Skicka data
    UDR0 = data;
}

void uart_println_uint16(uint16_t val) {
    char buffer[6]; // 0 till 65535 + null-terminering
    itoa(val, buffer, 10); // Konvertera till sträng (bas 10)
    
    // Skriv varje tecken
    for (char *p = buffer; *p != '\0'; p++) {
        uart_transmit(*p);
    }

    // Lägg till radbrytning
    uart_transmit('\r');
    uart_transmit('\n');
}
//############################################
//#                END UART                  #
//############################################

//############################################
//#                   ADC                    #
//############################################

// Se kapitel 23 Analog to digital converter

// Initiera ADC
void adc_init() {
    // ADMUX – ADC Multiplexer Selection Register
    // AVcc med extern kondensator på AREF (bit 6 i ADMUX sätts)
    ADMUX = (1 << REFS0);

    // ADCSRA – ADC Control and Status Register A
    // Se kapitel 23.9.2 ADCSRA – ADC Control and Status Register A i dokumentationen
    // Aktivera ADC, förklocka med 128 för 125 kHz vid 16 MHz CPU
    ADCSRA = (1 << ADEN)  | // ADC Enable
             (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler = 128
}

// Läs från ADC-kanal 0 (A0 = PC0)
uint16_t adc_read(uint8_t channel) {
    // ADMUX – ADC Multiplexer Selection Register
    // Välj kanal (bara de 3 sista bitarna används)
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    // ADCSRA – ADC Control and Status Register A
    // Se kapitel 23.9.2 ADCSRA – ADC Control and Status Register A i dokumentationen
    // ADSC - ADC Start Conversion
    // Starta konvertering
    ADCSRA |= (1 << ADSC);

    // Vänta tills konvertering är klar (ADSC blir 0)
    while (ADCSRA & (1 << ADSC));

    // Returnera resultat (10-bitars värde)
    return ADC;
}

//############################################
//#                 END  ADC                 #
//############################################

//############################################
//#                   MAIN                    #
//############################################

int main(void) {
    // Initiera ADC
    adc_init();

    // Initiera UART
    uart_init(103); // 9600 baud vid 16 MHz, se tabell 19.1 för uträkning (normal mode) 16.000.000/(16*9600) - 1 --> 103

    while (1) {
        uint16_t value = adc_read(0); // Läs från A0/PC0
        
        uart_println_uint16(value); // Här kan du skicka `value` vidare till UART
        _delay_ms(500);
    }
}
