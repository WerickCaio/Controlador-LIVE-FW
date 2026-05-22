#pragma once

// Inicializa o placar (lê EEPROM). Chame após HAL_Init().
void Scoreboard_Init();

// Persiste o placar atual na EEPROM.
void Scoreboard_Save();

// Zera todos os times e salva.
void Scoreboard_Clear();

// Adiciona `pontos` ao time `teamID` (0-based).
void Scoreboard_AddPoints(int teamID, int pontos);

// Subtrai `pontos` do time `teamID` (0-based), mínimo 0.
void Scoreboard_SubPoints(int teamID, int pontos);

// Funções de renderização (chame sempre na ordem: DrawTeams → DrawBoxes → DrawScores)
void Scoreboard_DrawTeams();
void Scoreboard_DrawBoxes();
void Scoreboard_DrawScores();
