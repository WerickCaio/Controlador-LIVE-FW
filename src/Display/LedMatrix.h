#pragma once
#include <Arduino.h>

// Cores universais do nosso motor gráfico
enum Colors { red, blue, green, yellow, cyan, purple, white };

// ======================================================================
// LED MATRIX: O MOTOR GRÁFICO
// Responsável apenas por desenhar píxeis e varrer a matriz de RAM.
// ======================================================================

void Display_Init();                       // Limpa a RAM e desenha as bordas brancas iniciais
void Display_Update();                     // Varre a matriz e envia para a HAL
void Display_Clear();                      // Apaga tudo (tela preta)

// Desenho Primitivo
void Display_PutPixel(int x, int y, int color);
void Display_ClearPixel(int x, int y, int color);

// Desenho de Caracteres (Usando o Drawing.h)
void Display_DrawChar(char c, int x, int y, int color);
void Display_DrawDigit(int numero, int x, int y, int color);
void Display_ClearCharArea(int x, int y, int color); // Limpa a área da letra (a sua antiga "vassoura")