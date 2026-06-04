#pragma once

void Scoreboard_Init();          
void Scoreboard_Save();          
void Scoreboard_Clear();
void Scoreboard_AddPoints(int cmd); 
void Scoreboard_SubPoints(int cmd);

void Scoreboard_SetTeamName(int teamId, const char* name);
void Scoreboard_SetScore(int teamId, int score);
void Scoreboard_SetTeamColor(int teamId, int color);
const char* Scoreboard_GetTeamName(int teamId);
int Scoreboard_GetScore(int teamId);
int Scoreboard_GetTeamColor(int teamId);

void Scoreboard_SetNumTeams(int num);
int Scoreboard_GetNumTeams();

void Scoreboard_SetBrightness(int brightness);
int Scoreboard_GetBrightness();

void Scoreboard_SetTestMode(int mode);
int Scoreboard_GetTestMode();
void Scoreboard_DrawTestPattern();

void Scoreboard_DrawTeams();
void Scoreboard_DrawScores();