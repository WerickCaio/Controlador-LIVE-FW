#include <Arduino.h>
#include "../include/Config.h"
#include "Hardware/Hal.h"
#include "Display/LedMatrix.h"
#include "Comms/Communication.h"
#include "Comms/WebServer.h"
#include "Game/Scoreboard.h"

enum stateMachine { test_hardware, start, idle };
uint8_t panelPlayerState = test_hardware; 

enum comandos {
  Ir_Para_Idle = 0,
  Cmd_AddPonto_Inicio = 1,  
  Cmd_AddPonto_Fim = 12,
  Cmd_SubPonto_Inicio = 13, 
  Cmd_SubPonto_Fim = 24,
  ZERA_O_PLACAR = 25,
  GUARDA_O_PLACAR = 26
};

void setup() {
    Comm_Init();
    HAL_Init();
    
    Display_Clear();     
    DEBUG_PRINTLN("=== Modo de Teste de Hardware Iniciado ===");
    
    WebServer_Init(); // Pode inicializar o Wi-Fi sem problemas
}

void loop() {
    int comandoRecebido = Comm_GetCommand();
    if (comandoRecebido == -1) {
        comandoRecebido = WebServer_GetCommand(); 
    }

    switch (panelPlayerState) {
        case test_hardware:
            Display_TestPattern();
            panelPlayerState = start; 
            DEBUG_PRINTLN(">>> Padrão de Teste Desenhado! Aguardando Comando 0...");
            break;

        case start:
            if (comandoRecebido == Ir_Para_Idle) {
                DEBUG_PRINTLN("[START] Comando 0 Recebido. Saindo do teste e indo pro Jogo!");
                
                Scoreboard_Init(); 
                Display_Clear();
                Scoreboard_DrawTeams(); 
                Scoreboard_DrawBoxes();
                Scoreboard_DrawScores();
                
                panelPlayerState = idle;
            }
            break;

        case idle:
            bool pontuacaoAlterada = false;

            if (comandoRecebido >= Cmd_AddPonto_Inicio && comandoRecebido <= Cmd_AddPonto_Fim) {
                Scoreboard_AddPoints(comandoRecebido - 1);
                pontuacaoAlterada = true;
            }
            else if (comandoRecebido >= Cmd_SubPonto_Inicio && comandoRecebido <= Cmd_SubPonto_Fim) {
                Scoreboard_SubPoints(comandoRecebido - 13);
                pontuacaoAlterada = true;
            }
            else if (comandoRecebido == ZERA_O_PLACAR) {
                Scoreboard_Clear();
                pontuacaoAlterada = true;
            }
            else if (comandoRecebido == GUARDA_O_PLACAR) {
                Scoreboard_Save();
                DEBUG_PRINTLN(">>> Placar Salvo na Memoria!");
            }
            else if (comandoRecebido == Ir_Para_Idle) {
                ESP.restart();
            }

            if (pontuacaoAlterada) {
                Display_Clear();
                Scoreboard_DrawTeams(); 
                Scoreboard_DrawBoxes();
                Scoreboard_DrawScores();
            }
            break;
    }

    Display_Update();
}