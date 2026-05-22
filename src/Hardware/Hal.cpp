#include "Hal.h"
#include "../../include/Config.h"

void HAL_Init() {
    pinMode(PIN_A,    OUTPUT);
    pinMode(PIN_B,    OUTPUT);
    pinMode(PIN_LAT,  OUTPUT);
    pinMode(PIN_OE,   OUTPUT);
    pinMode(PIN_CLK,  OUTPUT);
    pinMode(PIN_DATA, OUTPUT);

    // EEPROM.begin() deve ficar aqui (não em Scoreboard_Init)
    // para que a inicialização do hardware seja centralizada no HAL.
    EEPROM.begin(512);

    SPI.begin(PIN_CLK, -1, PIN_DATA, -1);
}

// ── Auto-detecção do tamanho do painel ───────────────────────────────
// Envia um pulso e conta quantos clocks até o sinal voltar pelo PIN_RETORNO.
// Retorna o número de shift registers × 16 (bits totais no daisy-chain).
// Útil para confirmar que o painel está ligado corretamente antes de exibir.
int HAL_PingHardware() {
    pinMode(PIN_DATA,    OUTPUT);
    pinMode(PIN_CLK,     OUTPUT);
    pinMode(PIN_OE,      OUTPUT);
    pinMode(PIN_RETORNO, INPUT_PULLDOWN);

    digitalWrite(PIN_OE,  HIGH);
    digitalWrite(PIN_CLK, LOW);

    // Limpa o shift register inteiro
    digitalWrite(PIN_DATA, LOW);
    for (int i = 0; i < 20000; i++) {
        digitalWrite(PIN_CLK, HIGH);
        digitalWrite(PIN_CLK, LOW);
    }

    // Envia um único '1' e conta clocks até ele retornar
    digitalWrite(PIN_DATA, HIGH);
    digitalWrite(PIN_CLK, HIGH);
    digitalWrite(PIN_CLK, LOW);
    digitalWrite(PIN_DATA, LOW);

    int contador = 1;
    while (digitalRead(PIN_RETORNO) == LOW && contador < 20000) {
        digitalWrite(PIN_CLK, HIGH);
        digitalWrite(PIN_CLK, LOW);
        contador++;
    }

    // Restaura o SPI
    SPI.end();
    SPI.begin(PIN_CLK, -1, PIN_DATA, -1);

    DEBUG_PRINTF("[HAL] PingHardware: %d bits detectados.\n", contador);
    return contador;
}

// ── Transferências SPI ────────────────────────────────────────────────
void HAL_SpiTransferNormal(uint16_t data) {
    SPI.beginTransaction(SPISettings(4000000, LSBFIRST, SPI_MODE0));
    SPI.transfer16(data);
    SPI.endTransaction();
}

void HAL_SpiTransferInverted(uint16_t data) {
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    SPI.transfer16(data);
    SPI.endTransaction();
}

// ── Controle do painel ────────────────────────────────────────────────
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
    digitalWrite(PIN_OE, LOW);  // Lógica invertida: LOW = ligado
}
