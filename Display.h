/*
 *  Display.h
 *  
 *  Gives some functions for working with a shift-register
    driven display. 
 */

/*
 spi_transfer: sends out one byte MSB first on USI. 
 Returns data clocked in during transfer. 
 */
uint8_t spi_transfer(uint8_t data) {
    USIDR = data;
    USISR = _BV(USIOIF); // clear flag
    
    while ( (USISR & _BV(USIOIF)) == 0 ) {
        USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC);
    }
    return USIDR;
}
void transferLong(unsigned int trans){
    /* Transfer the first half */
    spi_transfer(trans >> 8);
    /* Transfer the second half */
    spi_transfer((uint8_t)trans);
}
void updateHorizontal(int row, volatile unsigned int displayGreen[], volatile unsigned int displayRed[]){
    PORTB &= ~_BV(LATCH_PIN_HOR_GREEN);
    transferLong(~displayGreen[row]);
    PORTB |= _BV(LATCH_PIN_HOR_GREEN);
    
    PORTB &= ~_BV(LATCH_PIN_HOR_RED);
    transferLong(~displayRed[row]);
    PORTB |= _BV(LATCH_PIN_HOR_RED);
}
void updateVertical(int row){
    PORTB &= ~_BV(LATCH_PIN_VERT);
    spi_transfer(1 << row);
    PORTB |= _BV(LATCH_PIN_VERT);
}
//void outputDisplay(){
//    for(int r = 0; r < NUM_ROWS; r++){
//        Serial.println(displayGreen[r], HEX);
//    }
//    for(int r = 0; r < NUM_ROWS; r++){
//        Serial.println(displayRed[r], HEX);
//    }
//}
void scan(int row, volatile unsigned int displayGreen[], volatile unsigned int displayRed[]){
    updateVertical(row);
    updateHorizontal(row, displayGreen, displayRed);
}
/* Writes all zeros to display buffer. */
void clear(volatile unsigned int displayGreen[], volatile unsigned int displayRed[]){
    for(int i = 0; i < NUM_ROWS; i++){
        displayGreen[i] = 0;
        displayRed[i] = 0;
    }
}

/* Reverses the order of a byte. For example, 0x03 will become 0xC0. */
uint8_t reverse_byte(uint8_t b){
    uint8_t rev = 0;
    
    int i = 0;
    for (i = 0; i < 8; i++) {
        /* If the bit at position 7-i is 1 */
        if (b & (1 << (7 - i))) {
            /* Set the bit in i as 1*/
            rev |= (1 << i); 
        }
    }
    
    return rev;
}

