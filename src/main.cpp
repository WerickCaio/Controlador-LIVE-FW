#include <Arduino.h>
#include "../include/Config.h"
#include "Hardware/Hal.h"
#include "Display/LedMatrix.h"
#include "Comms/Communication.h"
#include "Game/Scoreboard.h"

// ======================================================================
// ESTADOS DA MÁQUINA PRINCIPAL
// ======================================================================
enum stateMachine { start, idle };
uint8_t panelPlayerState = start;

// Variáveis de controle de tempo e alternância de tela
unsigned long ultimaAtualizacao = 0;
bool showGroup2 = false; 

// ======================================================================
// MAPA DE COMANDOS (USB / BLUETOOTH)
// ======================================================================
enum comandos {
  Ir_Para_Idle = 0,
  Cmd_AddPonto_Inicio = 1,   // 1 ao 12: Soma pontos
  Cmd_AddPonto_Fim = 12,
  Cmd_SubPonto_Inicio = 13,  // 13 ao 24: Subtrai pontos
  Cmd_SubPonto_Fim = 24,
  ZERA_O_PLACAR = 25,
  GUARDA_O_PLACAR = 26
};

// ======================================================================
// FUNÇÃO AUXILIAR: FORÇAR DESENHO DA TELA ATUAL
// ======================================================================
void ForceRedraw() {
    Display_Clear();
    Scoreboard_DrawTeams(showGroup2);
    Scoreboard_DrawBoxes();
    Scoreboard_DrawScores(showGroup2);
}

// ======================================================================
// SETUP
// ======================================================================
void setup() {
    Comm_Init(); // Inicia a Serial USB e o Bluetooth HC-05
    HAL_Init();  // Inicia os pinos do painel e a EEPROM
    
    // Dispara o Sonar para auto-descobrir o tamanho físico do painel
    int bits = HAL_PingHardware();
    DEBUG_PRINTF("\n>>> Hardware Ping: %d bits <<<\n", bits);

    // Desenha a tela inicial de aguardo (bordas brancas)
    Display_Init();     
    Scoreboard_Init();  // Resgata os pontos antigos salvos na memória
    
    DEBUG_PRINTLN("=== Sistema Modular Iniciado ===");
    DEBUG_PRINTLN("Aguardando comando '0' para iniciar a Partida...");
}

// ======================================================================
// LOOP PRINCIPAL (O MAESTRO)
// ======================================================================
void loop() {
    unsigned long currentMillis = millis(); 
    
    // 1. Ouve o mundo exterior
    int comandoRecebido = Comm_GetCommand();

    // 2. Máquina de Estados
    switch (panelPlayerState) {
        
        case start:
            // Aguarda o comando 0 para começar o jogo
            if (comandoRecebido == Ir_Para_Idle) {
                DEBUG_PRINTLN("[START] Comando 0 Recebido. A Partida Comecou!");
                panelPlayerState = idle;
                showGroup2 = false; // Garante que começa sempre pelo grupo 1 (BUS/LAN)
                
                ForceRedraw();      // Apaga a borda e desenha o placar instantaneamente
                ultimaAtualizacao = currentMillis; // Inicia o timer de 3 segundos
            }
            break;

        case idle:
            bool pontuacaoAlterada = false;

            // --- LÓGICA DO JOGO ---
            if (comandoRecebido >= Cmd_AddPonto_Inicio && comandoRecebido <= Cmd_AddPonto_Fim) {
                Scoreboard_AddPoints(comandoRecebido - 1);
                pontuacaoAlterada = true;
                DEBUG_PRINTLN(">>> Ponto Adicionado!");
            }
            else if (comandoRecebido >= Cmd_SubPonto_Inicio && comandoRecebido <= Cmd_SubPonto_Fim) {
                Scoreboard_SubPoints(comandoRecebido - 13);
                pontuacaoAlterada = true;
                DEBUG_PRINTLN(">>> Ponto Removido!");
            }
            else if (comandoRecebido == ZERA_O_PLACAR) {
                Scoreboard_Clear();
                pontuacaoAlterada = true;
                DEBUG_PRINTLN(">>> Placar Zerado!");
            }
            else if (comandoRecebido == GUARDA_O_PLACAR) {
                Scoreboard_Save();
                DEBUG_PRINTLN(">>> Placar Salvo na EEPROM!");
            }
            else if (comandoRecebido == Ir_Para_Idle) {
                DEBUG_PRINTLN(">>> Comando 0 (Reset) Recebido. Reiniciando a placa...");
                ESP.restart();
            }

            // Se a pontuação mudou, atualiza a tela AGORA e zera o timer
            if (pontuacaoAlterada) {
                ForceRedraw();
                // Resetar a ultimaAtualizacao garante que a tela com o novo ponto 
                // fique visível por 3 segundos inteiros antes de alternar de equipa!
                ultimaAtualizacao = currentMillis; 
            }

            // --- ALTERNÂNCIA DE TELA (A cada 3 segundos) ---
            if (currentMillis - ultimaAtualizacao >= INTERVALO_EXIBICAO) {
                showGroup2 = !showGroup2; // Alterna entre as equipas
                ForceRedraw();
                ultimaAtualizacao = currentMillis;
            }
            break;
    }

    // 3. O motor físico nunca para de piscar a matriz de LED (Sem atrasos)
    Display_Update();
}