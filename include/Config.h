#pragma once
#include <Arduino.h>

// ======================================================================
//  CONFIGURAÇÃO CENTRAL DO PLACAR
//  Altere aqui para mudar o número de times, nomes e cores.
// ======================================================================

// --- Número de times (2, 3 ou 4) ---
#define NUM_TEAMS 4

// --- Cores disponíveis (use essas constantes em TIMES[]) ---
#define COR_VERMELHO  0   // red
#define COR_AZUL      1   // blue
#define COR_VERDE     2   // green
#define COR_AMARELO   3   // yellow
#define COR_CIANO     4   // cyan
#define COR_ROXO      5   // purple
#define COR_BRANCO    6   // white

// --- Estrutura de configuração de cada time ---
struct TeamConfig {
    char name[4];  // Até 3 letras + '\0'  (ex: "BUS", "TM1")
    int  color;    // Use as constantes COR_* acima
};

// Declaração extern — a definição fica em src/Game/TeamsConfig.cpp
extern const TeamConfig TIMES[NUM_TEAMS];

// ======================================================================
//  PINOS — ESP32-C3 Super Mini
// ======================================================================
constexpr int PIN_LAT      = 0;
constexpr int PIN_OE       = 1;
constexpr int PIN_B        = 3;
constexpr int PIN_A        = 4;
constexpr int PIN_CLK      = 6;
constexpr int PIN_DATA     = 7;
constexpr int PIN_RETORNO  = 2;   // Auto-detecção de tamanho do painel

// Pinos Serial para Bluetooth Clássico (HC-05)
// ⚠️ ATENÇÃO: O ESP32-C3 SÓ TEM BLE — o HC-05 (Classic) NÃO É COMPATÍVEL.
//    Para usar o HC-05, troque para o ESP32 DevKit clássico.
constexpr int RX_PIN = 20;
constexpr int TX_PIN = 21;

// ======================================================================
//  DEBUG
// ======================================================================
#define DEBUG_MODE 1   // Mude para 0 em produção para ganhar performance

#if DEBUG_MODE
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif
