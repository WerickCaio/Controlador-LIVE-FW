#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>

// ======================================================================
// HAL: HARDWARE ABSTRACTION LAYER
// Nenhuma lógica de jogo deve entrar aqui. Apenas controle físico de chips.
// ======================================================================

void HAL_Init();                     // Inicia os pinos, EEPROM e SPI
int  HAL_PingHardware();             // Faz o teste do Sonar e retorna os bits

// Funções de varredura do Painel
void HAL_SpiTransferNormal(uint16_t data);
void HAL_SpiTransferInverted(uint16_t data);
void HAL_LatchPanel();
void HAL_SetLinesPar();
void HAL_SetLinesImpar();
void HAL_DisableLines();