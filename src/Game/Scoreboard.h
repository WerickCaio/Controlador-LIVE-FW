#pragma once

void Scoreboard_Init();          
void Scoreboard_Save();          
void Scoreboard_Clear();
void Scoreboard_AddPoints(int cmd); 
void Scoreboard_SubPoints(int cmd);

void Scoreboard_DrawTeams();
void Scoreboard_DrawBoxes();
void Scoreboard_DrawScores();