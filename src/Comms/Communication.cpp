#include "Communication.h"
#include "../../include/Config.h" 
#include "../Hardware/Hal.h" // Incluído para rotear os comandos manuais
#include <Arduino.h>

void Comm_Init() {
    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
    Serial.setTimeout(20);
    Serial1.setTimeout(20);
    
    Serial.println(F("\n=== COMANDOS DE HARDWARE DISPONÍVEIS ==="));
    Serial.println(F("A ON/OFF | B ON/OFF | CLK ON/OFF | MOSI ON/OFF"));
    Serial.println(F("OE ON/OFF | LAT ON/OFF | PLOTLINHA | ZERODATA\n"));
}

int Comm_GetCommand() {
    String entrada = "";

    // 1. Checa a porta Serial do Cabo USB
    if (Serial.available() > 0) {
        entrada = Serial.readStringUntil('\n'); 
        entrada.trim(); 
        if (entrada.length() == 0) return -1; 
        
        // Se começar com uma letra, é comando de baixo nível (ex: "A ON", "ZERODATA")
        if (isAlpha(entrada[0])) {
            HAL_ExecuteDebug(entrada);
            return -1; // Retorna -1 para a máquina de estado do jogo ignorar
        }

        // Se for número, trata como comando de jogo
        int comando = entrada.toInt();
        if (comando == 0 && entrada != "0") return -1;
        return comando;
    }

    // 2. Checa o Módulo Bluetooth (O Bluetooth só envia comandos de jogo)
    if (Serial1.available() > 0) {
        entrada = Serial1.readStringUntil('\n');
        entrada.trim();
        if (entrada.length() == 0) return -1;
        
        int comando = entrada.toInt();
        if (comando == 0 && entrada != "0") return -1;
        return comando;
    }

    return -1;
}