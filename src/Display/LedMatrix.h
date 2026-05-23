#pragma once
#include <Arduino.h>

enum Colors
{
    red,
    blue,
    green,
    yellow,
    cyan,
    purple,
    white
};

void Display_Init();
void Display_Update();
void Display_Clear();

void Display_PutPixel(int x, int y, int color);
void Display_ClearPixel(int x, int y, int color);

void Display_DrawChar(char c, int x, int y, int color);
void Display_DrawDigit(int numero, int x, int y, int color);
void Display_ClearCharArea(int x, int y, int color);

void Display_TestPattern();             // <-- Modo de teste de Cascata
void Display_TestPattern(int offset_x); // <-- Adicione o parâmetro int