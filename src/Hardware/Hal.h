#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>

void HAL_Init();
int  HAL_PingHardware();

void HAL_SpiTransferNormal(uint16_t data);
void HAL_SpiTransferInverted(uint16_t data);
void HAL_LatchPanel();
void HAL_SetLinesPar();
void HAL_SetLinesImpar();
void HAL_DisableLines();
void HAL_EnableDisplay();            // NOVA: Acende o painel
void HAL_ExecuteDebug(String cmd);   // NOVA: Restaura os comandos manuais