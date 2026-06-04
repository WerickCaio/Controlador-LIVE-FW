#include "Hal.h"
#include "../include/Config.h"

void HAL_Init() {
    pinMode(PIN_R1, OUTPUT);
    pinMode(PIN_G1, OUTPUT);
    pinMode(PIN_B1, OUTPUT);
    pinMode(PIN_R2, OUTPUT);
    pinMode(PIN_G2, OUTPUT);
    pinMode(PIN_B2, OUTPUT);
    
    pinMode(PIN_A, OUTPUT);
    pinMode(PIN_B, OUTPUT);
    pinMode(PIN_LAT, OUTPUT);
    pinMode(PIN_OE, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    
    // Desativa o display na inicialização
    digitalWrite(PIN_OE, HIGH);
    
    EEPROM.begin(512);
}

void HAL_LatchPanel() {
    digitalWrite(PIN_LAT, HIGH);
    __asm__ __volatile__ ("nop; nop; nop; nop; nop;");
    digitalWrite(PIN_LAT, LOW);
}

void HAL_SetLinesPar() {
    digitalWrite(PIN_A, LOW);
    digitalWrite(PIN_B, HIGH);
}

void HAL_SetLinesImpar() {
    digitalWrite(PIN_B, LOW);
    digitalWrite(PIN_A, HIGH);
}

void HAL_DisableLines() {
    digitalWrite(PIN_OE, HIGH); 
}

void HAL_EnableDisplay() {
    digitalWrite(PIN_OE, LOW); // Acende a tela (Lógica invertida)
}