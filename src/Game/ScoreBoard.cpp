#include "Scoreboard.h"
#include "../Display/LedMatrix.h" 
#include <Arduino.h>
#include <Preferences.h>

Preferences preferences;

static int numTeams = 4;
static int teamsScore[4] = {0, 0, 0, 0};
static char teamNames[4][12] = {"BUS", "LAN", "WOO", "RAI"};
static int teamColors[4] = {red, blue, yellow, green};

static int testMode = 0;
static int testFrame = 0;
static unsigned long lastTestUpdate = 0;

void Scoreboard_SetTestMode(int mode) {
    testMode = mode;
    testFrame = 0;
    lastTestUpdate = 0;
}

int Scoreboard_GetTestMode() {
    return testMode;
}

void Scoreboard_SetTeamColor(int teamId, int color) {
    if (teamId >= 0 && teamId < 4) {
        teamColors[teamId] = color;
    }
}

int Scoreboard_GetTeamColor(int teamId) {
    if (teamId >= 0 && teamId < 4) return teamColors[teamId];
    return white;
}

void Scoreboard_SetBrightness(int brightness) {
    preferences.putInt("bright", brightness);
    Display_SetBrightness(brightness);
}

int Scoreboard_GetBrightness() {
    return preferences.getInt("bright", 1000);
}

void Scoreboard_SetNumTeams(int num) {
    if (num >= 2 && num <= 4) {
        numTeams = num;
        preferences.putInt("numTeams", num);
        Scoreboard_Clear();
    }
}

int Scoreboard_GetNumTeams() {
    return numTeams;
}

void Scoreboard_SetTeamName(int teamId, const char* name) {
    if (teamId >= 0 && teamId < 4) {
        strncpy(teamNames[teamId], name, 11);
        teamNames[teamId][11] = '\0';
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
    
    numTeams = preferences.getInt("numTeams", 4);
    if (numTeams < 2 || numTeams > 4) numTeams = 4;
    
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
        strncpy(teamNames[i], savedName.c_str(), 11);
        teamNames[i][11] = '\0';
        
        // Load Color
        String colorKey = "color" + String(i);
        teamColors[i] = preferences.getInt(colorKey.c_str(), teamColors[i]);
    }
    
    // Load Brightness
    int bright = preferences.getInt("bright", 1000);
    Display_SetBrightness(bright);
}

void Scoreboard_Save() {
    for (int i = 0; i < 4; i++) {
        String scoreKey = "score" + String(i);
        preferences.putInt(scoreKey.c_str(), teamsScore[i]);
        
        String nameKey = "name" + String(i);
        preferences.putString(nameKey.c_str(), teamNames[i]);
        
        String colorKey = "color" + String(i);
        preferences.putInt(colorKey.c_str(), teamColors[i]);
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

void Scoreboard_DrawTeams() {
    int y_cima = 2; // Offset vertical para a nova fonte 7x10
    int blockW = 128 / numTeams;
    
    for(int i = 0; i < numTeams; i++) {
        int len = strlen(teamNames[i]);
        if (len == 0) continue;
        
        // Cada letra tem 7px + 1px espaco = 8px (exceto a ultima)
        int stringWidth = len * 8 - 1;
        
        // Centralização horizontal matemática dentro do bloco
        int x_start = (i * blockW) + ((blockW - stringWidth) / 2);
        
        for(int c = 0; c < len; c++) {
            Display_DrawChar7x10(teamNames[i][c], x_start + (c * 8), y_cima, teamColors[i]);
        }
    }
}

void renderNumberInBox(int score, int blockX, int blockW, int y, int color) {
    int milhares = score / 1000;
    int centenas = (score % 1000) / 100;
    int dezenas  = (score % 100) / 10;
    int unidades = score % 10;
    
    int numDigits = (score >= 1000) ? 4 : (score >= 100) ? 3 : (score >= 10) ? 2 : 1;
    int strW = numDigits * 6 - 1;
    
    int dx = blockX + (blockW - strW) / 2;
    
    if (numDigits == 4) {
        Display_DrawDigit(milhares, dx, y, color); dx += 6;
        Display_DrawDigit(centenas, dx, y, color); dx += 6;
        Display_DrawDigit(dezenas,  dx, y, color); dx += 6;
        Display_DrawDigit(unidades, dx, y, color);
    } else if (numDigits == 3) {
        Display_DrawDigit(centenas, dx, y, color); dx += 6;
        Display_DrawDigit(dezenas,  dx, y, color); dx += 6;
        Display_DrawDigit(unidades, dx, y, color);
    } else if (numDigits == 2) {
        Display_DrawDigit(dezenas,  dx, y, color); dx += 6;
        Display_DrawDigit(unidades, dx, y, color);
    } else {
        Display_DrawDigit(unidades, dx, y, color);
    }
}

void Scoreboard_DrawScores() {
    int y_score = 19;
    int blockW = 128 / numTeams;
    
    for (int i = 0; i < numTeams; i++) {
        renderNumberInBox(teamsScore[i], i * blockW, blockW, y_score, white);
    }
}

void Scoreboard_DrawTestPattern() {
    if (testMode == 0) return;
    
    unsigned long now = millis();
    
    if (testMode == 1) { // 1 = Colunas
        if (now - lastTestUpdate > 20) { // Velocidade da varredura
            testFrame++;
            if (testFrame >= 128) testFrame = 0;
            lastTestUpdate = now;
            Display_Clear();
            for (int y = 0; y < 32; y++) {
                Display_PutPixel(testFrame, y, white);
            }
        }
    } 
    else if (testMode == 2) { // 2 = Linhas
        if (now - lastTestUpdate > 50) {
            testFrame++;
            if (testFrame >= 32) testFrame = 0;
            lastTestUpdate = now;
            Display_Clear();
            for (int x = 0; x < 128; x++) {
                Display_PutPixel(x, testFrame, white);
            }
        }
    }
    else if (testMode == 3) { // 3 = Pixel a Pixel
        if (now - lastTestUpdate > 2) {
            testFrame++;
            if (testFrame >= 128 * 32) testFrame = 0;
            lastTestUpdate = now;
            Display_Clear();
            int px = testFrame % 128;
            int py = testFrame / 128;
            Display_PutPixel(px, py, white);
        }
    }
    else if (testMode == 4) { // 4 = Cores
        if (now - lastTestUpdate > 1000) {
            testFrame++;
            if (testFrame >= 4) testFrame = 0;
            lastTestUpdate = now;
            Display_Clear();
            int color = (testFrame == 0) ? red : (testFrame == 1) ? green : (testFrame == 2) ? blue : white;
            for (int x = 0; x < 128; x++) {
                for (int y = 0; y < 32; y++) {
                    Display_PutPixel(x, y, color);
                }
            }
        }
    }
}