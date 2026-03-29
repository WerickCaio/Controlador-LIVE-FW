// =================================
// --- Especificações de projeto ---
// Projeto: ContadorLive
// Descrição: Objetivo do projeto será marcar um placar de modo que seja controlável com um controle remoto a adição e restart do placar a distância
// Projetista: Werick Caio
// Data: 07/02/24
//
// // ===================================
// // --- Inclusão das bibliotecas ---
#include <Arduino.h>
#include <LedControl.h>
// #include <util/delay.h>
// #include <SoftwareSerial.h>
#include "Debug.h"

// times para estarem no painel
/*
Land      - Verde  - LAN - Equipe 0
Wood     - Amarelo   - WOO - Equipe 1
Rain       - Azul      - RAI - Equipe 2
Bush        - Vermelho      - BUS - Equipe 3

Bush- Laranja
Land- Verde
Rain- Azul
Wood- Marrom

dinossaur   - verde     - DIN - Equipe liderança (Não pontua)
*/

// ==============================
// --- Definição de variáveis ---
unsigned long tempodecorrido = 0;
unsigned long tempodecorrido2 = 0;

unsigned long ultimaAtualizacao = 0; // Armazena o tempo da última atualização

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 500;  // the debounce time; increase if the output flickers

// #define DebounceTime 75

#define RX_PIN 20 // Exemplo de pino, pode mudar
#define TX_PIN 21 // Exemplo de pino, pode mudar

const int Quantidade_De_Comandos = 30;
const long intervaloDeExibicao = 3000; // 1 segundo para LED 1
const long intervalLed2 = 2000;        // 2 segundos para LED 2

// Define os pinos para SoftwareSerial
// SoftwareSerial bluetooth(3, 2); // RX, TX

enum alfabeto
{
  letraA,
  letraB,
  letraC,
  letraD,
  letraE,
  letraF,
  letraG,
  letraH,
  letraI,
  letraJ,
  letraK,
  letraL,
  letraM,
  letraN,
  letraO,
  letraP,
  letraQ,
  letraR,
  letraS,
  letraT,
  letraU,
  letraV,
  letraW,
  letraX,
  letraY,
  letraZ
};

enum stateMachine // indicará os estados da máquina de estado principal
{
  debug,
  testeBluetooth,
  start,
  initCounter,
  idle,
  plotBoxPlayer1,
  plotBoxPlayer2,
  plotBoxPlayer3,
  plotBoxPlayer4,
  colorConfigMove6,
  preSetCounterConfigured,
  config,
  finish,
  gravandoPlacar,
  zerandoAMemoria
};

enum colors
{
  red,
  blue,
  green,
  yellow,
  cyan,
  purple,
  white
};

enum comandos
{
  Ir_Para_Idle,
  Adicionar_Time_1_50,
  Adicionar_Time_1_100,
  Adicionar_Time_1_150,
  Adicionar_Time_2_50,
  Adicionar_Time_2_100,
  Adicionar_Time_2_150,
  Adicionar_Time_3_50,
  Adicionar_Time_3_100,
  Adicionar_Time_3_150,
  Adicionar_Time_4_50,
  Adicionar_Time_4_100,
  Adicionar_Time_4_150,
  Subtrair_Time_1_50,
  Subtrair_Time_1_100,
  Subtrair_Time_1_150,
  Subtrair_Time_2_50,
  Subtrair_Time_2_100,
  Subtrair_Time_2_150,
  Subtrair_Time_3_50,
  Subtrair_Time_3_100,
  Subtrair_Time_3_150,
  Subtrair_Time_4_50,
  Subtrair_Time_4_100,
  Subtrair_Time_4_150,
  ZERA_O_PLACAR,
  GUARDA_O_PLACAR
};

enum State
{
  STATE_1,
  STATE_2,
  STATE_3,
  STATE_4
};

State currentState = STATE_1; // Estado inicial
// void (*funcReset)() = 0;
// -----------------------------------------------------------------
// --- FUNÇÕES DE TESTE ---

int checkBluetoothCommands()
{
  String entrada = "";

  // 1. Checa a porta Serial do Cabo USB
  if (Serial.available() > 0)
  {
    entrada = Serial.readStringUntil('\n'); // Lê tudo até o Enter
    entrada.trim();                         // Arranca os invisíveis do CRLF e espaços

    if (entrada.length() == 0)
      return -1; // Se foi só um Enter vazio, ignora

    int comando = entrada.toInt();

    // Se o comando deu 0, mas o usuário NÃO digitou "0" (ex: digitou letras ou sujeira)
    if (comando == 0 && entrada != "0")
    {
      Serial.print("[AVISO] Comando Desconhecido (USB): ");
      Serial.println(entrada);
      return -1;
    }

    Serial.print(">>> Comando via USB Executado: ");
    Serial.println(comando);
    return comando;
  }

  // 2. Checa o Módulo Bluetooth (Pinos RX/TX)
  if (Serial1.available() > 0)
  {
    entrada = Serial1.readStringUntil('\n');
    entrada.trim();

    if (entrada.length() == 0)
      return -1;

    int comando = entrada.toInt();

    if (comando == 0 && entrada != "0")
    {
      Serial.print("[AVISO] Comando Desconhecido (BT): ");
      Serial.println(entrada);
      return -1;
    }

    Serial.print(">>> Comando via Bluetooth Executado: ");
    Serial.println(comando);
    return comando;
  }

  return -1; // Nenhum comando recebido
}

void plotNomeDasEquipes()
{
  // drawBoxTeams(0, const bool ch[], int color);
  // SetTheBoxes();
  static bool toggleFlagEquipes = 1;
  if (toggleFlagEquipes)
  {
    colocaLetra(0 + 11 * 0, 3, letraB, 0); //
    colocaLetra(0 + 11 * 1, 3, letraU, 0); //
    colocaLetra(0 + 11 * 2, 3, letraS, 0); //
                                           //
                                           //
                                           //
                                           //

    //

    // colocaLetra(-1 + 11 * 3, 3, letraN, 0); //
    // colocaLetra(-1 + 11 * 4, 3, letraI, 0); //
    // colocaLetra(-1 + 11 * 5, 3, letraS, 0); //
    colocaLetra(-1 + 11 * 3, 3, letraL, 2); //
    colocaLetra(-1 + 11 * 4, 3, letraA, 2); //
    colocaLetra(-1 + 11 * 5, 3, letraN, 2); //
  }
  else
  {
    colocaLetra(0 + 11 * 0, 3, letraW, 0); //
    colocaLetra(0 + 11 * 1, 3, letraO, 0); //
    colocaLetra(0 + 11 * 2, 3, letraO, 0); //
    colocaLetra(0 + 11 * 0, 3, letraW, 2); //
    colocaLetra(0 + 11 * 1, 3, letraO, 2); //
    colocaLetra(0 + 11 * 2, 3, letraO, 2); //
                                           //

    colocaLetra(-1 + 11 * 3, 3, letraR, 1); //
    colocaLetra(-1 + 11 * 4, 3, letraA, 1); //
    colocaLetra(-1 + 11 * 5, 3, letraI, 1); //
  }
  toggleFlagEquipes = !toggleFlagEquipes;
  // Continuar colocando os nomes agora
}

void plotCaixaPontuacao()
{
  for (int i = 0; i < 30; i++)
  {
    putPixelMemory(65 + i, 1, 6);
    putPixelMemory(65 + i, 14, 6);
    putPixelMemory(97 + i, 1, 6);
    putPixelMemory(97 + i, 14, 6);
  }
}

// -----------------------------------------------------------------

// bool detectLongPress(uint16_t aLongPressDurationMillis);
// =============================
void setup()
{
  // 1. Velocidade corrigida para igualar ao VS Code (115200)
  Serial.begin(115200);
  delay(1000); // Dá tempo para o monitor serial abrir e conectar

  ledDisplayBegin();

  // A comunicação com o HC-05 continua em 9600, isso está corretíssimo!
  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  Serial.setTimeout(20);
  Serial1.setTimeout(20);

  descobrirTamanhoDoPainel();

  // 2. Traz a tela à vida! Pinta a borda branca inicial
  clearPanelByPixel();
  initializerPanel(white);

  Serial.println("\n=== Sistema Iniciado com Sucesso ===");
  Serial.println("Envie o comando '0' para ir para o modo Jogo!");

  // Sintaxe corrigida (HIGH é estado, OUTPUT é modo)
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);
}

// ============================
// --- Função de repetição ---

// ============================
// --- Função de repetição ---
void loop()
{
  static uint8_t panelPlayerState = start;
  static uint8_t lastState = 255; // Variável para rastrear mudanças de estado

  // --- TELEMETRIA: Avisa no Monitor Serial sempre que o estado mudar ---
  if (panelPlayerState != lastState)
  {
    Serial.print("\n[MÁQUINA DE ESTADOS] Mudou para o estado: ");
    Serial.println(panelPlayerState);
    lastState = panelPlayerState;
  }

  // =============================================
  // --- Início da máquina de estado principal ---
  switch (panelPlayerState)
  {

  case testeBluetooth:
  {
    if (Serial1.available())
    {
      char command = Serial1.read();
      if (command != '\r' && command != '\n')
      {
        DEBUG_PRINT("Received: ");
        DEBUG_PRINTLN(command);
      }
    }
    if (Serial.available())
    {
      char c = Serial.read();
      Serial1.print("Recieved: ");
      Serial1.write(c);
    }
  }
  break;

  case debug:
  {
    DEBUG_PRINTLN("To debugando");
    digitalWrite(11, HIGH);
    digitalWrite(13, HIGH);
    delay(1);
    digitalWrite(13, LOW);

    digitalWrite(6, HIGH);
    delay(1);
    digitalWrite(6, LOW);
    delay(10);
  }
  break;

  case start: // Estado de inicialização para indicar que o painel Começou
  {
    if (checkBluetoothCommands() == Ir_Para_Idle)
    {
      Serial.println("[START] Recebeu comando 0! Indo para IDLE...");
      panelPlayerState = idle;
      
      // FORÇA a tela a atualizar IMEDIATAMENTE ao entrar no modo de jogo
      ultimaAtualizacao = millis() - intervaloDeExibicao; 
    }
  }
  break;

  case idle:
  { // Estado de aguardo de mudança do estado
    static int comandoRecebido = -1;
    unsigned long currentMillis = millis(); // Captura o tempo atual

    // --- ATUALIZAÇÃO DO PAINEL A CADA 3 SEGUNDOS ---
    if (currentMillis - ultimaAtualizacao >= intervaloDeExibicao)
    {
      ultimaAtualizacao = currentMillis; // Salva o tempo atual
      Serial.print("[IDLE] Atualizando tela! Mostrando equipes: ");
      Serial.println(currentState == STATE_1 ? "0 e 1 (BUS/LAN)" : "2 e 3 (WOO/RAI)");

      switch (currentState)
      {
      case STATE_1: // Mostra pontuação equipe 0 e 1
        clearPanelByPixel();
        plotNomeDasEquipes();
        plotCaixaPontuacao();
        counterPlacar(0);
        currentState = STATE_2;
        break;
      case STATE_2: // Mostra pontuação equipe 2 e 3
        clearPanelByPixel();
        plotNomeDasEquipes();
        plotCaixaPontuacao();
        counterPlacar(1);
        currentState = STATE_1;
        break;
      }
    }

    comandoRecebido = checkBluetoothCommands();

    switch (comandoRecebido)
    {
    case Ir_Para_Idle:
      Serial.println("[IDLE] Comando 0 recebido. Reiniciando ESP...");
      ESP.restart();
      panelPlayerState = start;
      break;
    case Adicionar_Time_1_50:
    case Adicionar_Time_1_100:
    case Adicionar_Time_1_150:
      Serial.println("[IDLE] Somando pontos ao Time 1");
      adicionaPontosEquipes(comandoRecebido - 1);
      ultimaAtualizacao = millis() - intervaloDeExibicao; // Atualiza tela na hora
      break;

    case Adicionar_Time_2_50:
    case Adicionar_Time_2_100:
    case Adicionar_Time_2_150:
      Serial.println("[IDLE] Somando pontos ao Time 2");
      adicionaPontosEquipes(comandoRecebido - 1);
      ultimaAtualizacao = millis() - intervaloDeExibicao;
      break;

    case Adicionar_Time_3_50:
    case Adicionar_Time_3_100:
    case Adicionar_Time_3_150:
      Serial.println("[IDLE] Somando pontos ao Time 3");
      adicionaPontosEquipes(comandoRecebido - 1);
      ultimaAtualizacao = millis() - intervaloDeExibicao;
      break;

    case Adicionar_Time_4_50:
    case Adicionar_Time_4_100:
    case Adicionar_Time_4_150:
      Serial.println("[IDLE] Somando pontos ao Time 4");
      adicionaPontosEquipes(comandoRecebido - 1);
      ultimaAtualizacao = millis() - intervaloDeExibicao;
      break;

    case Subtrair_Time_1_50:
    case Subtrair_Time_1_100:
    case Subtrair_Time_1_150:
      Serial.println("[IDLE] Subtraindo pontos do Time 1");
      subtraiPontosEquipes(comandoRecebido - 13);
      ultimaAtualizacao = millis() - intervaloDeExibicao;
      break;

    case Subtrair_Time_2_50:
    case Subtrair_Time_2_100:
    case Subtrair_Time_2_150:
      Serial.println("[IDLE] Subtraindo pontos do Time 2");
      subtraiPontosEquipes(comandoRecebido - 13);
      ultimaAtualizacao = millis() - intervaloDeExibicao;
      break;

    case Subtrair_Time_3_50:
    case Subtrair_Time_3_100:
    case Subtrair_Time_3_150:
      Serial.println("[IDLE] Subtraindo pontos do Time 3");
      subtraiPontosEquipes(comandoRecebido - 13);
      ultimaAtualizacao = millis() - intervaloDeExibicao;
      break;

    case Subtrair_Time_4_50:
    case Subtrair_Time_4_100:
    case Subtrair_Time_4_150:
      Serial.println("[IDLE] Subtraindo pontos do Time 4");
      subtraiPontosEquipes(comandoRecebido - 13);
      ultimaAtualizacao = millis() - intervaloDeExibicao;
      break;

    case ZERA_O_PLACAR:
      Serial.println("[IDLE] Zerando o Placar!");
      clearPlacarEEPROM();
      readPlacarEEPROM();
      ultimaAtualizacao = millis() - intervaloDeExibicao;
      break;

    case GUARDA_O_PLACAR:
      Serial.println("[IDLE] Salvando Placar na Memória!");
      savePlacarEEPROM();
      readPlacarEEPROM();
      break;
    }

    // Mantido o seu código original de teste de botões
    int buttonPressed = 1452;
    if (buttonPressed == 0x0C) toggleOutputEnable();
    if (buttonPressed == 0x10) maxPwmOutput();
    if (buttonPressed == 0x0D) changeFlagCounter(); 
    if (buttonPressed == 0x01) changeFlagState(0, 1); 
    if (buttonPressed == 0x02) changeFlagState(0, 2); 
    if (buttonPressed == 0x03) changeFlagState(0, 3); 
    if (buttonPressed == 0x04) changeFlagState(1, 1); 
    if (buttonPressed == 0x05) changeFlagState(1, 2); 
    if (buttonPressed == 0x06) changeFlagState(1, 3); 
    if (buttonPressed == 0x07) changeFlagState(2, 1); 
    if (buttonPressed == 0x08) changeFlagState(2, 2); 
    if (buttonPressed == 0x09) changeFlagState(2, 3); 
    if (buttonPressed == 0x46) changeFlagState(3, 1); 
    if (buttonPressed == 0xD9) changeFlagState(3, 3); 

    if (buttonPressed == 0x31) panelPlayerState = finish;
    if (buttonPressed == 0xBF) panelPlayerState = gravandoPlacar; 
    if (buttonPressed == 0x38) panelPlayerState = zerandoAMemoria; 
  }
  break; 

  case initCounter:
  {
    configScore();
    panelPlayerState = idle;
    Serial.println("I'm initcounter");
  }
  break;

  case preSetCounterConfigured:
  {
    SetTheBoxes();
    configScore();
    readPlacarEEPROM();
    panelPlayerState = idle;
    Serial.println("I'm preset");
  }
  break;

  case finish:
  {
    static bool OneTime = 1; 

    if (OneTime)
    {
      seeWhoWon();
      OneTime = 0;
    }
    int buttonPressed = 1452;
    if (buttonPressed == 0x2C) ESP.restart(); 
  }
  break;

  case gravandoPlacar:
  {
    savePlacarEEPROM();
    panelPlayerState = idle;
  }
  break;

  case zerandoAMemoria:
  {
    clearPlacarEEPROM();
    panelPlayerState = idle;
  }
  break;

  default:
  {
    // Estado de segurança
  }
  break;
  } // Fim da máquina de estado principal

  // ========================================
  // Atualiza o painel fisicamente sem parar
  updatePanel(); 
}

