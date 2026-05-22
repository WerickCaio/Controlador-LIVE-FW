#pragma once
#include <Arduino.h>

// Valores do enum coincidem com as constantes COR_* de Config.h
enum Colors { red, blue, green, yellow, cyan, purple, white };

void Display_Init();    // Desenha moldura de debug (opcional no setup)
void Display_Update();  // Varredura SPI — chame ao final de cada loop()
void Display_Clear();   // Apaga todo o framebuffer

void Display_PutPixel(int x, int y, int color);
void Display_ClearPixel(int x, int y, int color);

void Display_DrawChar(char c, int x, int y, int color);
void Display_DrawDigit(int numero, int x, int y, int color);
void Display_ClearCharArea(int x, int y, int color);

void Display_TestPattern(int offset_x);   // Animação de scroll no modo teste
