#include "Scoreboard.h"
#include "../../include/Config.h"
#include "../Display/LedMatrix.h"
#include <Arduino.h>
#include <EEPROM.h>

// ── Placar em memória ──────────────────────────────────────────────────
static int teamsScore[NUM_TEAMS];

// ======================================================================
//  ENGINE DE LAYOUT DINÂMICO
//
//  Tela: 128 × 32 px
//  ┌──────────────────────────────────────────────────────────────────┐
//  │  Área de nomes  (x: 0..63, 64px)  │  Área de scores (x: 64..127)│
//  └──────────────────────────────────────────────────────────────────┘
//
//  Distribuição de linhas:
//    1 linha (NUM_TEAMS ≤ 2): nomes em y=11, caixas y=1..30
//    2 linhas (NUM_TEAMS 3-4): linha 0 y=3/caixa 1..14 | linha 1 y=19/caixa 17..30
// ======================================================================

// Quantos times por linha (máximo 2 para o alfabeto de 10px caber)
static int teamsPerRow() {
    return (NUM_TEAMS <= 2) ? NUM_TEAMS : 2;
}

static int numRows() {
    int tpr = teamsPerRow();
    return (NUM_TEAMS + tpr - 1) / tpr;
}

// Y do texto do nome para cada linha lógica
static int rowNameY(int row) {
    return (numRows() == 1) ? 11 : (row == 0 ? 3 : 19);
}

// Y do topo e da base da caixa de score
static int rowBoxTop(int row) {
    return (numRows() == 1) ? 1 : (row == 0 ? 1 : 17);
}

static int rowBoxBottom(int row) {
    return (numRows() == 1) ? 30 : (row == 0 ? 14 : 30);
}

// X inicial do slot de nome para a coluna `col`
static int nameSlotX(int col) {
    return col * (64 / teamsPerRow());
}

// X da borda esquerda e direita da caixa de score
static int boxX_Start(int col) {
    return 64 + col * (64 / teamsPerRow());
}
static int boxX_End(int col) {
    return 64 + (col + 1) * (64 / teamsPerRow()) - 1;
}

// ── Renderiza número centrado dentro do espaço disponível ─────────────
static void renderNumberInBox(int score, int startX, int y, int color) {
    // Score limitado a 999 (3 dígitos)
    int centenas = score / 100;
    int dezenas  = (score % 100) / 10;
    int unidades = score % 10;

    if (score >= 100) {
        Display_DrawDigit(centenas, startX,      y, color);
        Display_DrawDigit(dezenas,  startX +  8, y, color);
        Display_DrawDigit(unidades, startX + 16, y, color);
    } else if (score >= 10) {
        Display_DrawDigit(dezenas,  startX + 4,  y, color);
        Display_DrawDigit(unidades, startX + 12, y, color);
    } else {
        Display_DrawDigit(unidades, startX + 8,  y, color);
    }
}

// ======================================================================
//  API PÚBLICA
// ======================================================================

void Scoreboard_Init() {
    // Nota: EEPROM.begin() é chamado em HAL_Init(). Não reordene no setup().
    for (int i = 0; i < NUM_TEAMS; i++) {
        EEPROM.get(i * sizeof(int), teamsScore[i]);
        if (teamsScore[i] < 0 || teamsScore[i] > 999) {
            teamsScore[i] = 0;
        }
    }
    DEBUG_PRINTLN("[Scoreboard] Placar carregado da EEPROM.");
}

void Scoreboard_Save() {
    for (int i = 0; i < NUM_TEAMS; i++) {
        EEPROM.put(i * sizeof(int), teamsScore[i]);
    }
    EEPROM.commit();
}

void Scoreboard_Clear() {
    for (int i = 0; i < NUM_TEAMS; i++) teamsScore[i] = 0;
    Scoreboard_Save();
    DEBUG_PRINTLN("[Scoreboard] Placar zerado e salvo.");
}

void Scoreboard_AddPoints(int teamID, int pontos) {
    if (teamID < 0 || teamID >= NUM_TEAMS) return;
    teamsScore[teamID] += pontos;
    if (teamsScore[teamID] > 999) teamsScore[teamID] = 999;
}

void Scoreboard_SubPoints(int teamID, int pontos) {
    if (teamID < 0 || teamID >= NUM_TEAMS) return;
    teamsScore[teamID] -= pontos;
    if (teamsScore[teamID] < 0) teamsScore[teamID] = 0;
}

// ── Renderização ──────────────────────────────────────────────────────

void Scoreboard_DrawTeams() {
    int tpr = teamsPerRow();
    for (int i = 0; i < NUM_TEAMS; i++) {
        int col   = i % tpr;
        int row   = i / tpr;
        int nameX = nameSlotX(col);
        int nameY = rowNameY(row);
        const char* nome = TIMES[i].name;
        int cor = TIMES[i].color;

        // Desenha até 3 letras; espaçamento de 11px por letra (10px + 1px gap)
        for (int j = 0; j < 3 && nome[j] != '\0'; j++) {
            Display_DrawChar(nome[j], nameX + j * 11, nameY, cor);
        }
    }
}

void Scoreboard_DrawBoxes() {
    int tpr   = teamsPerRow();
    int nRows = numRows();

    for (int row = 0; row < nRows; row++) {
        int yTop = rowBoxTop(row);
        int yBot = rowBoxBottom(row);

        // Quantos times nessa linha? (última linha pode ter menos)
        int teamsThisRow = min(tpr, NUM_TEAMS - row * tpr);

        for (int col = 0; col < teamsThisRow; col++) {
            int xL = boxX_Start(col);
            int xR = boxX_End(col);

            // Bordas horizontais (topo e base)
            for (int x = xL; x <= xR; x++) {
                Display_PutPixel(x, yTop, cyan);
                Display_PutPixel(x, yBot, cyan);
            }
            // Bordas verticais (esquerda e direita)
            for (int y = yTop + 1; y < yBot; y++) {
                Display_PutPixel(xL, y, cyan);
                Display_PutPixel(xR, y, cyan);
            }
        }
    }
}

void Scoreboard_DrawScores() {
    int tpr = teamsPerRow();
    for (int i = 0; i < NUM_TEAMS; i++) {
        int col    = i % tpr;
        int row    = i / tpr;
        int scoreX = boxX_Start(col) + 2;   // 2px de margem da borda esquerda
        int scoreY = rowBoxTop(row)  + 2;   // 2px abaixo da borda superior
        renderNumberInBox(teamsScore[i], scoreX, scoreY, white);
    }
}
