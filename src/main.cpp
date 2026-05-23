#include <Arduino.h>
#include "../include/Config.h"
#include "Hardware/Hal.h"
#include "Display/LedMatrix.h"
#include "Comms/Communication.h"
#include "Comms/PanelServer.h"
#include "Game/Scoreboard.h"

// ======================================================================
//  MÁQUINA DE ESTADOS DO PLACAR
// ======================================================================
//
//  TESTE_HARDWARE  → Rola texto de teste na tela.
//                    Qualquer comando 0 avança para AGUARDANDO_INICIO.
//
//  AGUARDANDO_INICIO → Tela em branco aguardando o operador pressionar
//                      "Iniciar Jogo" na web ou enviar 0 pela serial.
//
//  EM_JOGO         → Placar ativo. Aceita pontos, remoção, zero e save.
//
//  Protocolo de comandos (Wi-Fi / Serial):
//    0               → Iniciar jogo (de qualquer estado)
//    1 .. NUM_TEAMS  → +50 pts ao time (teamID = cmd - 1)
//  101 .. 100+N      → -50 pts do time (teamID = cmd - 101)
//  200               → Zerar placar
//  201               → Salvar placar na EEPROM
// ======================================================================

enum Estado : uint8_t
{
    TESTE_HARDWARE,
    AGUARDANDO_INICIO,
    EM_JOGO
};

static Estado estado = TESTE_HARDWARE;
static int scrollOffset = -40;
static unsigned long ultimoScroll = 0;

// ── Redesenha o placar completo na tela ──────────────────────────────
static void redesenharPlacar()
{
    Display_Clear();
    Scoreboard_DrawTeams();
    Scoreboard_DrawBoxes();
    Scoreboard_DrawScores();
}

// ======================================================================
//  SETUP
// ======================================================================
void setup()
{
    Comm_Init(); // Serial (e opcionalmente BT) — antes de qualquer print
    HAL_Init();  // Pinos, SPI, EEPROM — deve vir antes de Scoreboard_Init
    Display_Clear();

    DEBUG_PRINTLN("\n=== PLACAR ESCOLAR INICIADO ===");
    DEBUG_PRINTF("Times configurados: %d\n", NUM_TEAMS);
    for (int i = 0; i < NUM_TEAMS; i++)
    {
        DEBUG_PRINTF("  [%d] %s  (cor %d)\n", i, TIMES[i].name, TIMES[i].color);
    }

    PanelServer_Init(); // Sobe o AP Wi-Fi e servidor HTTP
    DEBUG_PRINTLN("[Setup] Modo: TESTE_HARDWARE. Aguardando cmd 0...");
}

void Debug_PixelMapper(int cmd)
{
    // cmd 300-999: acende o pixel de índice (cmd-300) varrendo linha por linha
    // Ex: cmd=300 → pixel (x=0,y=0) | cmd=301 → pixel (x=1,y=0) | cmd=332 → pixel (x=4,y=1)
    if (cmd < 300 || cmd > 300 + 128 * 32)
        return;

    int idx = cmd - 300;
    int x = idx % 128;
    int y = idx / 128;

    Display_Clear();
    Display_PutPixel(x, y, white);

    DEBUG_PRINTF("[MAP] idx=%d  logico=(x=%d, y=%d)\n", idx, x, y);
    DEBUG_PRINTLN("[MAP] Qual linha e coluna FISICA acendeu?");
}

// ======================================================================
//  LOOP
// ======================================================================
void loop()
{
    // Coleta o próximo comando (Serial tem prioridade sobre Wi-Fi)
    int cmd = Comm_GetCommand();
    if (cmd == -1)
    {
        cmd = PanelServer_GetCommand();
    }

    // ── TESTE_HARDWARE ───────────────────────────────────────────────
    if (estado == TESTE_HARDWARE)
    {
        if (millis() - ultimoScroll > 80)
        {
            scrollOffset++;
            if (scrollOffset > 128)
                scrollOffset = -40;
            Display_TestPattern(scrollOffset);
            ultimoScroll = millis();
        }

        if (cmd == 0)
        {
            DEBUG_PRINTLN("[Estado] AGUARDANDO_INICIO");
            Display_Clear();
            estado = AGUARDANDO_INICIO;
        }

        Display_Update();
        return; // Sai do loop aqui — evita o fall-through manual do original
    }

    // ── AGUARDANDO_INICIO ────────────────────────────────────────────
    if (estado == AGUARDANDO_INICIO)
    {
        if (cmd == 0)
        {
            DEBUG_PRINTLN("[Estado] EM_JOGO — carregando placar...");
            Scoreboard_Init(); // Lê EEPROM
            redesenharPlacar();
            estado = EM_JOGO;
        }
        Display_Update();
        return;
    }

    // ── EM_JOGO ──────────────────────────────────────────────────────
    if (estado == EM_JOGO)
    {
        bool atualizar = false;

        // +50 pts ao time (cmd 1 a NUM_TEAMS)
        if (cmd >= 1 && cmd <= NUM_TEAMS)
        {
            int teamID = cmd - 1;
            Scoreboard_AddPoints(teamID, 50);
            atualizar = true;
            DEBUG_PRINTF("[Jogo] +50 → %s\n", TIMES[teamID].name);
        }
        // -50 pts do time (cmd 101 a 100+NUM_TEAMS)
        else if (cmd >= 101 && cmd <= 100 + NUM_TEAMS)
        {
            int teamID = cmd - 101;
            Scoreboard_SubPoints(teamID, 50);
            atualizar = true;
            DEBUG_PRINTF("[Jogo] -50 → %s\n", TIMES[teamID].name);
        }
        // Zerar placar
        else if (cmd == 200)
        {
            Scoreboard_Clear();
            atualizar = true;
            DEBUG_PRINTLN("[Jogo] Placar zerado.");
        }
        // Salvar placar na EEPROM
        else if (cmd == 201)
        {
            Scoreboard_Save();
            DEBUG_PRINTLN("[Jogo] Placar salvo na EEPROM.");
        }
        // Reiniciar (volta para teste de hardware)
        else if (cmd == 0)
        {
            DEBUG_PRINTLN("[Jogo] Reiniciando...");
            ESP.restart();
        }
        // Adicione temporariamente no loop(), dentro do case EM_JOGO:
        else if (cmd == 99)
        {
            int bits = HAL_PingHardware();
            DEBUG_PRINTF("[DIAG] Bits detectados no daisy-chain: %d\n", bits);
            DEBUG_PRINTF("[DIAG] Esperado para 128x32: %d\n", 128 * 3); // 3 planos RGB

            // Testa um pixel por vez nos 4 cantos
            Display_Clear();
            Display_PutPixel(0, 0, red);
            delay(500);
            Display_Update();
            Display_PutPixel(127, 0, green);
            delay(500);
            Display_Update();
            Display_PutPixel(0, 31, blue);
            delay(500);
            Display_Update();
            Display_PutPixel(127, 31, white);
            delay(500);
            Display_Update();
        }
        else if (cmd >= 300 && cmd <= 4396)
        {
            Debug_PixelMapper(cmd);
        }

        if (atualizar)
        {
            redesenharPlacar();
        }

        Display_Update();
    }
}
