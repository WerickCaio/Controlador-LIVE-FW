#include "Hal.h"
#include "../include/Config.h"

void HAL_Init() {
    pinMode(PIN_A, OUTPUT);
    pinMode(PIN_B, OUTPUT);
    pinMode(PIN_LAT, OUTPUT);
    pinMode(PIN_OE, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_DATA, OUTPUT);
    
    EEPROM.begin(512);
    SPI.begin(PIN_CLK, -1, PIN_DATA, -1);
}

int HAL_PingHardware() {
    pinMode(PIN_DATA, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_OE, OUTPUT);
    pinMode(PIN_RETORNO, INPUT_PULLDOWN);

    digitalWrite(PIN_OE, HIGH); 
    digitalWrite(PIN_CLK, LOW);
    
    digitalWrite(PIN_DATA, LOW);
    for(int i = 0; i < 20000; i++) {
        digitalWrite(PIN_CLK, HIGH); digitalWrite(PIN_CLK, LOW);
    }

    digitalWrite(PIN_DATA, HIGH);
    digitalWrite(PIN_CLK, HIGH); digitalWrite(PIN_CLK, LOW);
    digitalWrite(PIN_DATA, LOW);
    
    int contador_de_bits = 1;
    while(digitalRead(PIN_RETORNO) == LOW && contador_de_bits < 20000) {
        digitalWrite(PIN_CLK, HIGH); digitalWrite(PIN_CLK, LOW);
        contador_de_bits++;
    }

    SPI.end();
    SPI.begin(PIN_CLK, -1, PIN_DATA, -1);
    return contador_de_bits;
}

void HAL_SpiTransferNormal(uint16_t data) {
    // Reduzimos a velocidade de 4000000 para 500000 para evitar reflexão!
    SPI.beginTransaction(SPISettings(500000, LSBFIRST, SPI_MODE0)); 
    SPI.transfer16(data);
    SPI.endTransaction();
}

void HAL_SpiTransferInverted(uint16_t data) {
    // Reduzimos a velocidade de 4000000 para 500000 para evitar reflexão!
    SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
    SPI.transfer16(data);
    SPI.endTransaction();
}

void HAL_LatchPanel() {
    digitalWrite(PIN_LAT, HIGH);
    digitalWrite(PIN_LAT, LOW);
}

void HAL_SetLinesPar() {
    digitalWrite(PIN_A, HIGH);
    digitalWrite(PIN_B, LOW);
}

void HAL_SetLinesImpar() {
    digitalWrite(PIN_B, HIGH);
    digitalWrite(PIN_A, LOW);
}

void HAL_DisableLines() {
    digitalWrite(PIN_A, HIGH);
    digitalWrite(PIN_B, HIGH);
    digitalWrite(PIN_OE, HIGH); 
}

void HAL_EnableDisplay() {
    digitalWrite(PIN_OE, LOW); // Acende a tela (Lógica invertida)
}