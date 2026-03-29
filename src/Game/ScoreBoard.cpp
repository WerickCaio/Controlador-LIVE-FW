#include "Scoreboard.h"
#include "../Display/LedMatrix.h" // Chama o motor gráfico para desenhar
#include <Arduino.h>
#include <EEPROM.h>

// ======================================================================
// VARIÁVEIS PROTEGIDAS (ESTADO DO JOGO)
// ======================================================================
// teamsScore[0] = BUS  |  teamsScore[1] = LAN
// teamsScore[2] = WOO  |  teamsScore[3] = RAI
static int teamsScore[4] = {0, 0, 0, 0};

// ======================================================================
// GESTÃO DE MEMÓRIA (EEPROM)
// ======================================================================
void Scoreboard_Init() {
    // Lê os 4 inteiros da memória (cada int ocupa 4 bytes, por isso i * 4)
    for (int i = 0; i < 4; i++) {
        EEPROM.get(i * sizeof(int), teamsScore[i]);
        
        // Proteção: Se a memória for nova/corrompida, zera o valor
        if (teamsScore[i] < 0 || teamsScore[i] > 9999) {
            teamsScore[i] = 0;
        }
    }
}

void Scoreboard_Save() {
    for (int i = 0; i < 4; i++) {
        EEPROM.put(i * sizeof(int), teamsScore[i]);
    }
    EEPROM.commit(); // Obrigatório no ESP32 para confirmar a gravação física
}

void Scoreboard_Clear() {
    for (int i = 0; i < 4; i++) {
        teamsScore[i] = 0;
    }
    Scoreboard_Save();
}

// ======================================================================
// LÓGICA DE PONTUAÇÃO (A MÁQUINA MATEMÁTICA)
// ======================================================================
void Scoreboard_AddPoints(int cmd) {
    // O cmd vem de 0 a 11 (referente aos comandos Adicionar_Time_X_YY)
    // cmd 0, 1, 2 = Time 0 | cmd 3, 4, 5 = Time 1 | etc...
    int timeID = cmd / 3; 
    
    // cmd%3 resulta em 0, 1 ou 2. Multiplicando por 50 temos 50, 100, 150!
    int pontosGanhos = ((cmd % 3) + 1) * 50; 

    teamsScore[timeID] += pontosGanhos;

    // Trava o placar num máximo de 999 para não bugar o ecrã
    if (teamsScore[timeID] > 999) teamsScore[timeID] = 999;
}

void Scoreboard_SubPoints(int cmd) {
    // A mesma matemática aplicada à subtração
    int timeID = cmd / 3; 
    int pontosPerdidos = ((cmd % 3) + 1) * 50; 

    teamsScore[timeID] -= pontosPerdidos;

    // Não permite pontuação negativa
    if (teamsScore[timeID] < 0) teamsScore[timeID] = 0;
}

// ======================================================================
// RENDERIZAÇÃO GRÁFICA (A PONTE COM O DISPLAY)
// ======================================================================
void Scoreboard_DrawBoxes() {
    // Desenha as caixas ao redor dos números na metade direita da tela
    for (int i = 0; i < 30; i++) {
        Display_PutPixel(65 + i, 1, cyan);  // Teto da Caixa 1
        Display_PutPixel(65 + i, 14, cyan); // Chão da Caixa 1
        
        Display_PutPixel(97 + i, 1, cyan);  // Teto da Caixa 2
        Display_PutPixel(97 + i, 14, cyan); // Chão da Caixa 2
    }
}

void Scoreboard_DrawTeams(bool isGroup2) {
    int y_pos = 3; // Linha vertical base para as letras

    if (!isGroup2) {
        // --- EQUIPES 0 e 1 (BUS / LAN) ---
        // Desenhando "BUS" na cor Vermelha
        Display_DrawChar('B', 0,  y_pos, red);
        Display_DrawChar('U', 11, y_pos, red);
        Display_DrawChar('S', 22, y_pos, red);

        // Desenhando "LAN" na cor Azul
        Display_DrawChar('L', 34, y_pos, blue);
        Display_DrawChar('A', 45, y_pos, blue);
        Display_DrawChar('N', 56, y_pos, blue);
    } else {
        // --- EQUIPES 2 e 3 (WOO / RAI) ---
        // Desenhando "WOO" na cor Amarela
        Display_DrawChar('W', 0,  y_pos, yellow);
        Display_DrawChar('O', 11, y_pos, yellow);
        Display_DrawChar('O', 22, y_pos, yellow);

        // Desenhando "RAI" na cor Verde
        Display_DrawChar('R', 34, y_pos, green);
        Display_DrawChar('A', 45, y_pos, green);
        Display_DrawChar('I', 56, y_pos, green);
    }
}

// Função auxiliar interna para fatiar o número e centralizar na caixa
void renderNumberInBox(int score, int startX, int y, int color) {
    int centenas = score / 100;
    int dezenas  = (score % 100) / 10;
    int unidades = score % 10;

    // Um dígito tem 5 px de largura. Espaçamento de 3 px = 8 px por salto.
    if (score >= 100) {
        // Centralizado para 3 dígitos
        Display_DrawDigit(centenas, startX, y, color);
        Display_DrawDigit(dezenas,  startX + 8, y, color);
        Display_DrawDigit(unidades, startX + 16, y, color);
    } 
    else if (score >= 10) {
        // Centralizado para 2 dígitos (empurramos +4px para a direita)
        Display_DrawDigit(dezenas,  startX + 4, y, color);
        Display_DrawDigit(unidades, startX + 12, y, color);
    } 
    else {
        // Centralizado para 1 dígito (empurramos +8px para a direita)
        Display_DrawDigit(unidades, startX + 8, y, color);
    }
}

void Scoreboard_DrawScores(bool isGroup2) {
    int y_pos = 4; // Posição vertical dos números dentro da caixa

    if (!isGroup2) {
        // Mostra o placar dos times 0 e 1
        renderNumberInBox(teamsScore[0], 69, y_pos, white); // Caixa Esquerda
        renderNumberInBox(teamsScore[1], 101, y_pos, white); // Caixa Direita
    } else {
        // Mostra o placar dos times 2 e 3
        renderNumberInBox(teamsScore[2], 69, y_pos, white); // Caixa Esquerda
        renderNumberInBox(teamsScore[3], 101, y_pos, white); // Caixa Direita
    }
}