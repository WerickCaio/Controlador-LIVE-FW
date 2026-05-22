#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>

// Inicializa pinos, SPI e EEPROM.
// ⚠️  Deve ser chamada ANTES de Scoreboard_Init().
void HAL_Init();

// Auto-detecção do tamanho do painel via pulso SPI.
// Retorna o número de bits detectados (equivale ao número de shift registers × 16).
int  HAL_PingHardware();

// Funções de varredura do painel
void HAL_SpiTransferNormal(uint16_t data);
void HAL_SpiTransferInverted(uint16_t data);
void HAL_LatchPanel();
void HAL_SetLinesPar();
void HAL_SetLinesImpar();
void HAL_DisableLines();
void HAL_EnableDisplay();
