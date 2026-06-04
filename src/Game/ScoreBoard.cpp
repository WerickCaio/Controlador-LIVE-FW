#include "Scoreboard.h"
#include "../Display/LedMatrix.h" 
#include <Arduino.h>
#include <Preferences.h>

Preferences preferences;

static int teamsScore[4] = {0, 0, 0, 0};
static char teamNames[4][4] = {"BUS", "LAN", "WOO", "RAI"};
static int teamColors[4] = {red, blue, yellow, green};

void Scoreboard_SetTeamName(int teamId, const char* name) {
    if (teamId >= 0 && teamId < 4) {
        strncpy(teamNames[teamId], name, 3);
        teamNames[teamId][3] = '\0';
    }
}

void Scoreboard_SetScore(int teamId, int score) {
    if (teamId >= 0 && teamId < 4) {
        teamsScore[teamId] = score;
        if (teamsScore[teamId] < 0) teamsScore[teamId] = 0;
        if (teamsScore[teamId] > 9950) teamsScore[teamId] = 9950;
    }
}

const char* Scoreboard_GetTeamName(int teamId) {
    if (teamId >= 0 && teamId < 4) return teamNames[teamId];
    return "";
}

int Scoreboard_GetScore(int teamId) {
    if (teamId >= 0 && teamId < 4) return teamsScore[teamId];
    return 0;
}

void Scoreboard_Init() {
    preferences.begin("acamp", false);
    
    const char* defaultNames[4] = {"BUS", "LAN", "WOO", "RAI"};
    for (int i = 0; i < 4; i++) {
        // Load Score
        String scoreKey = "score" + String(i);
        teamsScore[i] = preferences.getInt(scoreKey.c_str(), 0);
        if (teamsScore[i] < 0 || teamsScore[i] > 9950) {
            teamsScore[i] = 0;
        }
        
        // Load Name
        String nameKey = "name" + String(i);
        String savedName = preferences.getString(nameKey.c_str(), defaultNames[i]);
        strncpy(teamNames[i], savedName.c_str(), 3);
        teamNames[i][3] = '\0';
    }
}

void Scoreboard_Save() {
    for (int i = 0; i < 4; i++) {
        String scoreKey = "score" + String(i);
        preferences.putInt(scoreKey.c_str(), teamsScore[i]);
        
        String nameKey = "name" + String(i);
        preferences.putString(nameKey.c_str(), teamNames[i]);
    }
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
    if (teamsScore[timeID] > 9950) teamsScore[timeID] = 9950;
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
    for(int i = 0; i < 4; i++) {
        int x_start = i * 32;
        // Centralização do texto no bloco (3 letras = 32 pixels, cada letra é 10px, +1 espaco = 11px per char -> total 31px. Sobra 1px.
        int char_spacing = 11;
        
        for(int c = 0; c < 3; c++) {
            if(teamNames[i][c] != '\0') {
                // Desenha a letra convertendo para upper caso precise, mas o painel ja espera char normal
                Display_DrawChar(teamNames[i][c], x_start + (c * char_spacing), y_cima, teamColors[i]);
            }
        }
    }
}

void renderNumberInBox(int score, int blockX, int y, int color) {
    int milhares = score / 1000;
    int centenas = (score % 1000) / 100;
    int dezenas  = (score % 100) / 10;
    int unidades = score % 10;
    
    // Auto-centralização dentro de um bloco de 32 pixels
    // Largura total de 4 digitos = 29px (5+3+5+3+5+3+5). Sobra 1.5px de lado
    if (score >= 1000) {
        Display_DrawDigit(milhares, blockX + 1, y, color);
        Display_DrawDigit(centenas, blockX + 9, y, color);
        Display_DrawDigit(dezenas,  blockX + 17, y, color);
        Display_DrawDigit(unidades, blockX + 25, y, color);
    } else if (score >= 100) {
        // 3 digitos = 21px. Sobra 5.5px -> offset 5
        Display_DrawDigit(centenas, blockX + 5, y, color);
        Display_DrawDigit(dezenas,  blockX + 13, y, color);
        Display_DrawDigit(unidades, blockX + 21, y, color);
    } else if (score >= 10) {
        // 2 digitos = 13px. Sobra 9.5px -> offset 9
        Display_DrawDigit(dezenas,  blockX + 9, y, color);
        Display_DrawDigit(unidades, blockX + 17, y, color);
    } else {
        // 1 digito = 5px. Sobra 13.5px -> offset 13
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