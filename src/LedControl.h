// =================================
// --- Especificações de arquivo ---
// Arquivo .h das funções
// Projetista: Werick Caio

#ifndef LedControl
#define LedControl 

// ===================================
// --- Inclusão das bibliotecas ---
#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>

// -- Funções para melhorar a compreensão do código --
void latchPanel();
void ativaLinhaPar();
void ativaLinhaImpar();
void desativaTudo();
// void melquisedeque(); // Função de Toggle do estado do pino / Usado quando a função de SPI é chamada
void toggleOE(); // Ativa ou desativa o pino OE, mas se torna obsoleto quando utilizado o analogWrite

// -- Funções importantes do código
void ledDisplayBegin(); // Inicia todas as configurações necessárias para o início da máquina de estado
void updatePanel(); // Função responsável por permanecer exibindo o valor na variável ledPanelRgb
void changeFlagState(uint8_t team, uint8_t quantity);

 
// Funções básicas gerais --
void fillPanel();
void fillShiftRegisters();
void cleanShiftRegisters();

// -- Funções Gráficas Primitivas--
void putPixelMemory(int x, int y, int color); // Coloca um ponto em qualquer no painel
void clearPixelMemory(int x, int y, int color); // Limpa um ponto qualquer no painel

void drawBoxTeams(int team, const bool ch[], int color);
void drawBoxTeams42x16(int team, const bool ch[], int color); //função teste para o novo painel do acampamento

void colocaNumero(int xi, int yi, int numero, int color); // Função responsável por mostrar os números no placar



// -- Funções gráficas Alto nível
void SetTheBoxes(); // Configura no painel os times vermelho, verde azul e amarelo (respectivamente)
void SetTheBoxesExtra();
void configScore();

void colocaDigitoPlacarRGB(int xi, int yi, int numero);
void LimpaDigitoPlacarRGB(int xi, int yi);

void counterPlacar(int team);
void alteraPontosTeam(uint8_t team);

void callBox(int team, bool flagRed, bool flagGreen, bool flagBlue, bool flagYellow) ;
void defineBox(int team, bool flagRed, bool flagGreen, bool flagBlue, bool flagYellow);



// -- Funções guardadas para próximas versões --
void configPWM(int intensidade); // Vou deixar, pois ainda tenho que aprender a fazer ele funcionar, então será um legacy



int checkControl();
void initializerPanel(int color);


void clearPanelByPixel();
void toggleOutputEnable();
void maxPwmOutput();
void colocaLetra(int xi, int yi, int letra, int color); // Função para a plotagem de números no placar
void changeFlagCounter();
void seeWhoWon();
int findMax(int a, int b, int c, int d);


void drawBoxMove(int team, const bool ch[], int color); // função para desenhar as caixas dos números
void  drawBoxMovePanel(int color);

void plotMinistry(int team);
void clearMinistryBox();

void checaATrocaDePaineis();

void readPlacarEEPROM();
void savePlacarEEPROM();
void clearPlacarEEPROM();

void adicionaPontosEquipes(int time);
void subtraiPontosEquipes(int time);

void ComandosSerial();
void PlotLinha();
void ZeroData();





#endif