#include <Arduino.h>
#include "../include/Config.h"
#include "Hardware/Hal.h"
#include "Display/LedMatrix.h"
#include "Comms/Communication.h"
#include "Comms/WebServer.h"
#include "Game/Scoreboard.h"

// ====================================================================
// TASK 1: Atualização do Display (DEDICADA AO CORE 1)
// ====================================================================
void TaskDisplay(void *pvParameters) {
    for (;;) {
        Display_Update();
        
        // Yield pequeno para evitar o acionamento do Watchdog Timer (WDT)
        // Isso dá 1ms de respiro para o RTOS manter a casa em ordem
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

// ====================================================================
// TASK 0: Servidor Web e Lógica do Jogo (DEDICADA AO CORE 0)
// ====================================================================
void TaskWeb(void *pvParameters) {
    for (;;) {
        WebServer_GetCommand(); // Processa requisições HTTP do Dashboard
        
        // Se alguma API web alterou os pontos ou nomes, redesenhamos o painel
        if (WebServer_NeedsRedraw()) {
            Display_Clear();
            Scoreboard_DrawTeams();
            Scoreboard_DrawBoxes();
            Scoreboard_DrawScores();
        }
        
        // Yield generoso (10ms) pois a Web não precisa de tempo real extremo
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup()
{
    // Inicialização do Sistema
    Comm_Init();
    HAL_Init();

    Display_Clear();
    DEBUG_PRINTLN("=== Inicializando o Jogo ===");

    WebServer_Init(); // Sobe o AP "ACAMP_VOX"

    // Desenha o estado inicial do placar
    Scoreboard_Init();
    Scoreboard_DrawTeams();
    Scoreboard_DrawBoxes();
    Scoreboard_DrawScores();

    // ====================================================================
    // MULTITHREADING (FreeRTOS)
    // ====================================================================
    
    // Core 0: Ficará com a conectividade Wi-Fi e a lógica HTTP Web
    xTaskCreatePinnedToCore(
        TaskWeb,
        "TaskWeb",
        8192,
        NULL,
        1,       // Prioridade Normal
        NULL,
        0        // Core 0
    );

    // Core 1: Ficará focado APENAS em cuspir os bits pro painel HUB75
    xTaskCreatePinnedToCore(
        TaskDisplay,
        "TaskDisplay",
        8192,
        NULL,
        10,      // Prioridade ALTÍSSIMA
        NULL,
        1        // Core 1
    );
}

void loop()
{
    // O loop padrão do Arduino roda no Core 1 com prioridade 1.
    // Como criamos Tasks dedicadas profissionais, não precisamos mais do loop padrão.
    // Deletar o loop principal economiza recursos.
    vTaskDelete(NULL);
}