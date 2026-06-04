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
    // Linha horizontal dividindo Nomes e Pontos
    for (int x = 0; x < 128; x++) {
        Display_PutPixel(x, 15, cyan);
    }
    // Linhas verticais separando os 4 times
    for (int y = 0; y < 32; y++) {
        Display_PutPixel(31, y, cyan);
        Display_PutPixel(63, y, cyan);
        Display_PutPixel(95, y, cyan);
    }
}

void Scoreboard_DrawTeams() {
    int y_cima = 3;
    
    // Team 0: BUS
    Display_DrawChar('B', 0,  y_cima, red);
    Display_DrawChar('U', 11, y_cima, red);
    Display_DrawChar('S', 22, y_cima, red);

    // Team 1: LAN
    Display_DrawChar('L', 33, y_cima, blue);
    Display_DrawChar('A', 44, y_cima, blue);
    Display_DrawChar('N', 55, y_cima, blue);

    // Team 2: WOO
    Display_DrawChar('W', 65, y_cima, yellow);
    Display_DrawChar('O', 76, y_cima, yellow);
    Display_DrawChar('O', 87, y_cima, yellow);

    // Team 3: RAI
    Display_DrawChar('R', 97, y_cima, green);
    Display_DrawChar('A', 108, y_cima, green);
    Display_DrawChar('I', 119, y_cima, green);
}

void renderNumberInBox(int score, int blockX, int y, int color) {
    int centenas = score / 100;
    int dezenas  = (score % 100) / 10;
    int unidades = score % 10;
    
    // Auto-centralização dentro de um bloco de 32 pixels
    if (score >= 100) {
        Display_DrawDigit(centenas, blockX + 5, y, color);
        Display_DrawDigit(dezenas,  blockX + 13, y, color);
        Display_DrawDigit(unidades, blockX + 21, y, color);
    } else if (score >= 10) {
        Display_DrawDigit(dezenas,  blockX + 9, y, color);
        Display_DrawDigit(unidades, blockX + 17, y, color);
    } else {
        Display_DrawDigit(unidades, blockX + 13, y, color);
    }
}

void Scoreboard_DrawScores() {
    int y_score = 19;
    renderNumberInBox(teamsScore[0], 0,  y_score, white);
    renderNumberInBox(teamsScore[1], 32, y_score, white);
    renderNumberInBox(teamsScore[2], 64, y_score, white);
    renderNumberInBox(teamsScore[3], 96, y_score, white);
}