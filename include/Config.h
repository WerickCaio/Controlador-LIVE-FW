#pragma once
#include <Arduino.h>

// ======================================================================
// CONFIGURAÇÕES GLOBAIS E PINOS (ESP32-C3)
// ======================================================================

// Pinos do Painel de LED
constexpr int PIN_R1 = 11;
constexpr int PIN_G1 = 12;
constexpr int PIN_B1 = 10;
constexpr int PIN_R2 = 7;
constexpr int PIN_G2 = 15;
constexpr int PIN_B2 = 6;
constexpr int PIN_A = 18;
constexpr int PIN_B = 8;
constexpr int PIN_CLK = 9;
constexpr int PIN_LAT = 46;
constexpr int PIN_OE = 3;

// Pinos de Comunicação Serial (Módulo Bluetooth HC-05)
constexpr int RX_PIN = 20;
constexpr int TX_PIN = 21;

// Configurações do Jogo
constexpr long INTERVALO_EXIBICAO = 3000; // Tempo de alternância de tela em ms

// Sistema de Debug (Defina como 0 para calar o terminal e ganhar performance)
#define DEBUG_MODE 1  

#if DEBUG_MODE
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif