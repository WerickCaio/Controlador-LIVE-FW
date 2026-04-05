#pragma once
#include <Arduino.h>

// ======================================================================
// CONFIGURAÇÕES GLOBAIS E PINOS (ESP32-C3)
// ======================================================================

// Pinos do Painel de LED (SPI e Controle)
constexpr int PIN_LAT = 0;
constexpr int PIN_OE = 1;
constexpr int PIN_B = 3;
constexpr int PIN_A = 4;
constexpr int PIN_CLK = 6;
constexpr int PIN_DATA = 7;

// Pino do Sonar (Auto-detecção de tamanho do painel)
constexpr int PIN_RETORNO = 2;

// Pinos de Comunicação Serial (Módulo Bluetooth HC-05)
constexpr int RX_PIN = 20;
constexpr int TX_PIN = 21;

// Configurações do Jogo
constexpr long INTERVALO_EXIBICAO = 3000; // Tempo de alternância de tela em ms

// Sistema de Debug (Defina como 0 para calar o terminal e ganhar performance)
#define DEBUG_MODE 1

#if DEBUG_MODE
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(...)
#endif