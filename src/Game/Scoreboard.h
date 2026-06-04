#pragma once

void Scoreboard_Init();          
void Scoreboard_Save();          
void Scoreboard_Clear();
void Scoreboard_AddPoints(int cmd); 
void Scoreboard_SubPoints(int cmd);

void Scoreboard_SetTeamName(int teamId, const char* name);
void Scoreboard_SetScore(int teamId, int score);
const char* Scoreboard_GetTeamName(int teamId);
int Scoreboard_GetScore(int teamId);

void Scoreboard_DrawTeams();
void Scoreboard_DrawBoxes();
void Scoreboard_DrawScores();