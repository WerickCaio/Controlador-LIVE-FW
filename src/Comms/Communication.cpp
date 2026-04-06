#include "Communication.h"
#include "../include/Config.h" // Corrigido o caminho para a pasta include
#include <Arduino.h>

void Comm_Init() {
    Serial.begin(115200);
    // Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Descomente se usar o HC-05
    Serial.setTimeout(20);
    // Serial1.setTimeout(20);
}

int Comm_GetCommand() {
    String entrada = "";
    
    // 1. Checa a porta Serial do Cabo USB
    if (Serial.available() > 0) {
        entrada = Serial.readStringUntil('\n');
        entrada.trim(); 
        
        if (entrada.length() == 0) return -1; 
        
        int comando = entrada.toInt();
        
        if (comando == 0 && entrada != "0") {
            DEBUG_PRINTLN("[AVISO] Comando Desconhecido (USB)");
            return -1;
        }
        return comando;
    }

    // 2. Checa o Módulo Bluetooth (Se estiver usando)
    /*
    if (Serial1.available() > 0) {
        entrada = Serial1.readStringUntil('\n');
        entrada.trim();
        if (entrada.length() == 0) return -1;
        
        int comando = entrada.toInt();
        if (comando == 0 && entrada != "0") {
            DEBUG_PRINTLN("[AVISO] Comando Desconhecido (BT)");
            return -1;
        }
        return comando;
    }
    */

    return -1; // Nenhum comando recebido
}