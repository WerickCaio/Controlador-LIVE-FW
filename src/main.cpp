#include <Arduino.h>
#include "../include/Config.h"
#include "Hardware/Hal.h"
#include "Display/LedMatrix.h"
#include "Comms/Communication.h"
#include "Game/Scoreboard.h"

// Estados do Jogo
enum stateMachine { start, idle };
uint8_t panelPlayerState = start;

// Controle de Alternância de Tela
unsigned long ultimaAtualizacao = 0;
bool showGroup2 = false; 

// Comandos do Controle (Mantidos do seu código original)
enum comandos {
  Ir_Para_Idle = 0,
  Cmd_AddPonto_Inicio = 1,  // Comandos 1 ao 12 são de soma
  Cmd_AddPonto_Fim = 12,
  Cmd_SubPonto_Inicio = 13, // Comandos 13 ao 24 são de subtração
  Cmd_SubPonto_Fim = 24,
  ZERA_O_PLACAR = 25,
  GUARDA_O_PLACAR = 26
};

void setup() {
    Comm_Init();
    HAL_Init();
    
    int bits = HAL_PingHardware();
    DEBUG_PRINTF("\n>>> Hardware Ping: %d bits <<<\n", bits);

    Display_Init();     // Limpa a RAM e desenha as bordas
    Scoreboard_Init();  // Lê as pontuações antigas
    
    DEBUG_PRINTLN("=== Sistema Modular Iniciado ===");
}

void loop() {
    unsigned long currentMillis = millis(); 
    int comandoRecebido = Comm_GetCommand();

    switch (panelPlayerState) {
        
        case start:
            if (comandoRecebido == Ir_Para_Idle) {
                DEBUG_PRINTLN("[START] Comando 0 Recebido. Indo para Jogo!");
                panelPlayerState = idle;
                ultimaAtualizacao = currentMillis - INTERVALO_EXIBICAO; // Força desenhar na hora
            }
            break;

        case idle:
            // 1. Renderiza a tela a cada X segundos
            if (currentMillis - ultimaAtualizacao >= INTERVALO_EXIBICAO) {
                ultimaAtualizacao = currentMillis;
                showGroup2 = !showGroup2; // Alterna entre Time 0/1 e Time 2/3
                
                Display_Clear();
                Scoreboard_DrawTeams(showGroup2);
                Scoreboard_DrawBoxes();
                Scoreboard_DrawScores(showGroup2);
            }

            // 2. Processa as Regras do Jogo
            if (comandoRecebido >= Cmd_AddPonto_Inicio && comandoRecebido <= Cmd_AddPonto_Fim) {
                Scoreboard_AddPoints(comandoRecebido - 1);
                ultimaAtualizacao = currentMillis - INTERVALO_EXIBICAO; // Atualiza a tela agora!
            }
            else if (comandoRecebido >= Cmd_SubPonto_Inicio && comandoRecebido <= Cmd_SubPonto_Fim) {
                Scoreboard_SubPoints(comandoRecebido - 13);
                ultimaAtualizacao = currentMillis - INTERVALO_EXIBICAO;
            }
            else if (comandoRecebido == ZERA_O_PLACAR) {
                Scoreboard_Clear();
                ultimaAtualizacao = currentMillis - INTERVALO_EXIBICAO;
            }
            else if (comandoRecebido == GUARDA_O_PLACAR) {
                Scoreboard_Save();
                DEBUG_PRINTLN("Placar Salvo na Memoria!");
            }
            else if (comandoRecebido == Ir_Para_Idle) {
                DEBUG_PRINTLN("Reiniciando o sistema...");
                ESP.restart();
            }
            break;
    }

    // 3. Atualiza os LEDs físicos incansavelmente, sem nunca travar!
    Display_Update();
}