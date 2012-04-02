/* 
	Shows how to use SPI with the ATtiny85 USI. 
 */

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>

const int LATCH_PIN_VERT = 0;
const int LATCH_PIN_HOR_GREEN = 3;
const int LATCH_PIN_HOR_RED = 4;
const int NUM_ROWS = 8;
const int NUM_COLS = 16;
const int REFRESH_PERIOD = 1000/60;

#include "Display.h"

/* Global variable for timer interrupt. */
volatile unsigned long millis = 0;

int main(void) {
	
    /*** Begin display variables */
    volatile unsigned int displayGreen[NUM_ROWS];
    volatile unsigned int displayRed[NUM_ROWS];
    /*** End display variables */
    
    /* Set latch pins high */
    PORTB |= _BV(LATCH_PIN_VERT);
    PORTB |= _BV(LATCH_PIN_HOR_GREEN);
    PORTB |= _BV(LATCH_PIN_HOR_RED);
    
    /* Set up all pins as outputs */
    DDRB |= _BV(PB1); /* DO: Data out */
    DDRB |= _BV(PB2); /* USCK: USI clock */
    DDRB |= _BV(LATCH_PIN_VERT);
    DDRB |= _BV(LATCH_PIN_HOR_GREEN);
    DDRB |= _BV(LATCH_PIN_HOR_RED);     
    
    /** Set registers for 1ms timer interrupts */
    /* p82 Clear Timer on Compare Match (CTC) Mode */
    TCCR0A |= (1 << WGM01); 
    /* Set clock prescaler to CLK/64. */
    TCCR0B |= (1 << CS01) | (1 << CS00);
    /* Set output compare to 125. This encodes a resolution of 
     8Mhz/64/125 = 1000 Hz = 1ms to timer 0. */
    OCR0A = 125;
    /* Enable interrupt bit for output compare flag 0 A */
    TIMSK |= (1 << OCIE0A);
    /* Enable interrupts */
    sei();
    
    while(1) {
        int currentRow = 0;
        unsigned long lastScreenRefresh = 0;
        unsigned long lastScan = 0;
        
		while (1) {
            /* Generate image for display. */
            if (millis - lastScreenRefresh > REFRESH_PERIOD) {
                /* Generate sine wave */
                clear(displayGreen, displayRed);
                
                double PERIOD = 2200;
                double COLS_PER_PERIOD = 19;
                /* Display Green, then red, then both */
                if(millis % 6000 < 2000){ 
                    for(int c = 0; c < NUM_COLS; c++){
                        int r = (int)round(square((2.628 * sin((millis + c * (PERIOD / COLS_PER_PERIOD))/PERIOD * 2 * M_PI))));
                        displayGreen[r] |= 1 << c;
                    }
                }
                else if(millis % 6000 < 4000) {
                    for(int c = 0; c < NUM_COLS; c++){
                        int r = (int)round(square((2.628 * sin((millis + c * (PERIOD / COLS_PER_PERIOD))/PERIOD * 2 * M_PI))));
                        displayRed[r] |= 1 << c;
                    }
                }
                else {
                    for(int c = 0; c < NUM_COLS; c++){
                        int r = (int)round(square((2.628 * sin((millis + c * (PERIOD / COLS_PER_PERIOD))/PERIOD * 2 * M_PI))));
                        displayGreen[r] |= 1 << c;
                        displayRed[r] |= 1 << c;
                    }
                }    
                lastScreenRefresh = millis;
            }
            /* Scan rows of display */
            if (millis - lastScan > REFRESH_PERIOD / NUM_ROWS) {
                scan(currentRow, displayGreen, displayRed);
                
                currentRow++;
                currentRow %= NUM_ROWS;
                lastScan = millis;
            }
        }
	}
};

ISR(TIMER0_COMPA_vect){
	millis++;	
}
    