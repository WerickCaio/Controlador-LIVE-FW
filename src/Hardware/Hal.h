#pragma once
#include <Arduino.h>
#include <EEPROM.h>

void HAL_Init();                     

// Funções de varredura do Painel
void HAL_LatchPanel();
void HAL_SetLinesPar();
void HAL_SetLinesImpar();
void HAL_DisableLines();
void HAL_EnableDisplay();