#pragma once

void Scoreboard_Init();          // Inicia e lê a EEPROM
void Scoreboard_Save();          // Salva na EEPROM
void Scoreboard_Clear();         // Zera e salva
void Scoreboard_AddPoints(int cmd); // Soma pontos (0 a 11)
void Scoreboard_SubPoints(int cmd); // Subtrai pontos (0 a 11)

// Desenhos do Placar (Usando o Motor Gráfico)
void Scoreboard_DrawTeams(bool isGroup2);
void Scoreboard_DrawBoxes();
void Scoreboard_DrawScores(bool isGroup2);