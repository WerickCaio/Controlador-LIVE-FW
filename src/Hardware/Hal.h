#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>

void HAL_Init();
int HAL_PingHardware();

// Funções de varredura do Painel
void HAL_SpiTransferNormal(uint16_t data);
void HAL_SpiTransferInverted(uint16_t data);
void HAL_LatchPanel();
void HAL_SetLinesPar();
void HAL_SetLinesImpar();
void HAL_DisableLines();
void HAL_EnableDisplay(); // <-- Nova função do brilho