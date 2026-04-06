#include "Scoreboard.h"
#include "../Display/LedMatrix.h" 
#include <Arduino.h>
#include <EEPROM.h>

static int teamsScore[4] = {0, 0, 0, 0};

void Scoreboard_Init() {
    for (int i = 0; i < 4; i++) {
        EEPROM.get(i * sizeof(int), teamsScore[i]);
        if (teamsScore[i] < 0 || teamsScore[i] > 9999) {
            teamsScore[i] = 0;
        }
    }
}

void Scoreboard_Save() {
    for (int i = 0; i < 4; i++) {
        EEPROM.put(i * sizeof(int), teamsScore[i]);
    }
    EEPROM.commit(); 
}

void Scoreboard_Clear() {
    for (int i = 0; i < 4; i++) {
        teamsScore[i] = 0;
    }
    Scoreboard_Save();
}

void Scoreboard_AddPoints(int cmd) {
    int timeID = cmd / 3;
    int pontosGanhos = ((cmd % 3) + 1) * 50; 
    teamsScore[timeID] += pontosGanhos;
    if (teamsScore[timeID] > 999) teamsScore[timeID] = 999;
}

void Scoreboard_SubPoints(int cmd) {
    int timeID = cmd / 3;
    int pontosPerdidos = ((cmd % 3) + 1) * 50; 
    teamsScore[timeID] -= pontosPerdidos;
    if (teamsScore[timeID] < 0) teamsScore[timeID] = 0;
}

// Renderização Fixa para os 4 times (128x32)
void Scoreboard_DrawBoxes() {
    // Caixas Cima
    for (int i = 0; i < 30; i++) {
        Display_PutPixel(65 + i, 1, cyan);
        Display_PutPixel(65 + i, 14, cyan);
        Display_PutPixel(97 + i, 1, cyan);
        Display_PutPixel(97 + i, 14, cyan);
    }
    // Caixas Baixo
    for (int i = 0; i < 30; i++) {
        Display_PutPixel(65 + i, 17, cyan);
        Display_PutPixel(65 + i, 30, cyan);
        Display_PutPixel(97 + i, 17, cyan);
        Display_PutPixel(97 + i, 30, cyan);
    }
}

void Scoreboard_DrawTeams() {
    int y_cima = 3;
    Display_DrawChar('B', 0,  y_cima, red);
    Display_DrawChar('U', 11, y_cima, red);
    Display_DrawChar('S', 22, y_cima, red);

    Display_DrawChar('L', 34, y_cima, blue);
    Display_DrawChar('A', 45, y_cima, blue);
    Display_DrawChar('N', 56, y_cima, blue);

    int y_baixo = 19;
    Display_DrawChar('W', 0,  y_baixo, yellow);
    Display_DrawChar('O', 11, y_baixo, yellow);
    Display_DrawChar('O', 22, y_baixo, yellow);

    Display_DrawChar('R', 34, y_baixo, green);
    Display_DrawChar('A', 45, y_baixo, green);
    Display_DrawChar('I', 56, y_baixo, green);
}

void renderNumberInBox(int score, int startX, int y, int color) {
    int centenas = score / 100;
    int dezenas  = (score % 100) / 10;
    int unidades = score % 10;
    
    if (score >= 100) {
        Display_DrawDigit(centenas, startX, y, color);
        Display_DrawDigit(dezenas,  startX + 8, y, color);
        Display_DrawDigit(unidades, startX + 16, y, color);
    } else if (score >= 10) {
        Display_DrawDigit(dezenas,  startX + 4, y, color);
        Display_DrawDigit(unidades, startX + 12, y, color);
    } else {
        Display_DrawDigit(unidades, startX + 8, y, color);
    }
}

void Scoreboard_DrawScores() {
    renderNumberInBox(teamsScore[0], 69, 4, white);
    renderNumberInBox(teamsScore[1], 101, 4, white);
    renderNumberInBox(teamsScore[2], 69, 20, white);
    renderNumberInBox(teamsScore[3], 101, 20, white);
}