#include "Communication.h"
#include "../../include/Config.h"
#include <Arduino.h>

// ======================================================================
//  COMUNICAÇÃO SERIAL
//
//  Protocolo de comandos (envie via monitor serial ou app BT):
//    0             → Iniciar jogo / reiniciar
//    1..NUM_TEAMS  → +50 pts ao time (teamID = cmd - 1)
//  101..100+N      → -50 pts do time (teamID = cmd - 101)
//  200             → Zerar placar
//  201             → Salvar placar na EEPROM
//
//  Envie o número seguido de '\n' (Enter). Ex: "2\n" → +50 ao time 2.
// ======================================================================

void Comm_Init() {
    Serial.begin(115200);
    Serial.setTimeout(20);

    // ── Bluetooth Clássico (HC-05) ─────────────────────────────────
    // ⚠️  O ESP32-C3 tem apenas BLE — HC-05 (Bluetooth Clássico) é INCOMPATÍVEL.
    //    Para usar HC-05, troque para o ESP32 DevKit clássico e descomente abaixo:
    //
    // Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
    // Serial1.setTimeout(20);
}

int Comm_GetCommand() {
    // 1. USB Serial (cabo)
    if (Serial.available() > 0) {
        String entrada = Serial.readStringUntil('\n');
        entrada.trim();
        if (entrada.length() == 0) return -1;

        int cmd = entrada.toInt();
        if (cmd == 0 && entrada != "0") {
            DEBUG_PRINTLN("[Serial] Comando desconhecido ignorado.");
            return -1;
        }
        DEBUG_PRINTF("[Serial] Comando recebido: %d\n", cmd);
        return cmd;
    }

    // 2. Bluetooth Clássico — descomente se usar Serial1:
    /*
    if (Serial1.available() > 0) {
        String entrada = Serial1.readStringUntil('\n');
        entrada.trim();
        if (entrada.length() == 0) return -1;
        int cmd = entrada.toInt();
        if (cmd == 0 && entrada != "0") return -1;
        return cmd;
    }
    */

    return -1;
}
