// =================================
// --- Especificações de arquivo ---
// Arquivo .cpp das funções
// Projetista: Werick Caio

// ===============================
// --- Inclusão de bibliotecas ---
#include <LedControl.h>
#include <Drawing.h>

// =================================
// --- Mapeamento de Hardware ---
// Pinos do ESP32-C3
constexpr int PIN_LAT = 0;
constexpr int PIN_OE = 1;
constexpr int PIN_B = 3;
constexpr int PIN_A = 4;
constexpr int PIN_CLK = 6;
constexpr int PIN_DATA = 7;

// ==============================
// --- Definição de variáveis ---
// Primeiro de tudo, a variável referente aos primeiros 64 bits de informações está sendo declarada a seguir
// uint64_t ledPanelRgb[2][48]; // referente aos 3072 pontos de luz [ 0 - 1023 = vermelho; 1024 - 2047 = Verde; 2048 - 3071 = Azul;
// Com a adição do novo painel, a variável terá que ser uma matriz, ficando na forma de:
uint64_t ledPanelRgb[2][48];
int teamsScore[6] = {0, 0, 0, 0, 0, 0}; // Variável que guarda o valor dos 6 times == Tenho que ver uma posição na memória da EEPROM PRA GUARDAR ESSES VALORES.
// Ver também como guardar o último estado do painel antes de ele ser desligado.

unsigned long tempoParaTrocaDoPlacar = 0;
unsigned millisTarefa1 = 0;
unsigned millisTarefa2 = 0;
bool flagPonto[6] = {0, 0, 0, 0, 0, 0};
bool flagCounter = 1;
bool mudancaDoPainelSeis = 1;
uint8_t flagQuantify = 1;

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

// ==============================
// --- Estrutura das funções ---
void ledDisplayBegin() // Função de configuração inicial
{
    // Configuração das portas
    pinMode(PIN_A, OUTPUT);
    pinMode(PIN_B, OUTPUT);
    pinMode(PIN_LAT, OUTPUT);
    pinMode(PIN_OE, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_DATA, OUTPUT);

    EEPROM.begin(512); // O ESP32 exige alocar o tamanho da EEPROM na RAM primeiro

    SPI.begin(PIN_CLK, -1, PIN_DATA, -1); // Inicia SPI nos pinos certos do ESP32
    // Configurações gráficas iniciais
    initializerPanel(white); // Desenha a Caixa branca em volta de tudo
    // drawBoxMovePanel(cyan);
    // SPI.begin();          // Inicia a comunicação SPI
    // Desenha Move na inicialização do painel
    colocaLetra(5 + 11 * 0, 4, letraA, red);
    colocaLetra(5 + 11 * 1, 4, letraC, red);
    colocaLetra(5 + 11 * 2, 4, letraA, red);
    colocaLetra(5 + 11 * 3, 4, letraM, red);
    colocaLetra(5 + 11 * 4, 4, letraP, red);

    colocaLetra(5 + 11 * 0, 4, letraA, green);
    colocaLetra(5 + 11 * 1, 4, letraC, green);
    colocaLetra(5 + 11 * 2, 4, letraA, green);
    colocaLetra(5 + 11 * 3, 4, letraM, green);
    colocaLetra(5 + 11 * 4, 4, letraP, green);

    colocaLetra(5 + 11 * 0, 4, letraA, blue);
    colocaLetra(5 + 11 * 1, 4, letraC, blue);
    colocaLetra(5 + 11 * 2, 4, letraA, blue);
    colocaLetra(5 + 11 * 3, 4, letraM, blue);
    colocaLetra(5 + 11 * 4, 4, letraP, blue);

    // EEPROM.write(0, 256);
    // EEPROM.write(1, 6);
    // EEPROM.write(2, 128);
    // EEPROM.write(4, 32);
    // EEPROM.write(6, 49);
    // EEPROM.write(8, 62);
    // EEPROM.write(10, 77);
    // Agora eu posso ter certeza de que ele está pegando do endereõ correto
    // analogWrite(6, 240);
    // PORTD &= ~OE;
    readPlacarEEPROM();
}

void clearPanelByPixel() // Zera a variável que é exibida no painel
{
    for (size_t j = 0; j < 128; j++)
    {
        for (size_t i = 0; i < 16; i++)
        {
            clearPixelMemory(j, i, white);
        }
    }
}

void fillPanel() // preenche a variável que é exibida no painel
{
    for (size_t j = 0; j < 2; j++)
    {
        for (size_t i = 0; i < 48; i++)
        {
            ledPanelRgb[j][i] = ~0;
        }
    }
}

// =====================================================
// --- Funções para melhorar a compreensão do código ---
void latchPanel()
{
    digitalWrite(PIN_LAT, HIGH);
    digitalWrite(PIN_LAT, LOW);
}

void ativaLinhaImpar()
{
    digitalWrite(PIN_B, HIGH);
    digitalWrite(PIN_A, LOW);
}

void ativaLinhaPar()
{
    digitalWrite(PIN_A, HIGH);
    digitalWrite(PIN_B, LOW);
}

void toggleOE()
{
    // Lê o estado atual e inverte (substitui o bitshift maluco do Uno)
    digitalWrite(PIN_OE, !digitalRead(PIN_OE));
}

void desativaTudo()
{
    digitalWrite(PIN_B, HIGH);
    digitalWrite(PIN_A, HIGH);
}
// ===========================================
// --- Funções de funcionamento de projeto ---
void updatePanel()
{
    // Serial.print("Estou no UPDATE");
    // Essa é a função mais importante, ela varre a variável que guarda as informações de modo que o valor sempre seja atualizado, permitindo a leitura dessa variável
    // Variável que é varrida se chama ledPanelRgb[2], ela é uma matriz de uint64_t com 48 linhas, referentes ao painel de led mostrado
    // A disposição de linhas é da seguinte maneira
    // ledPanelRgb[2][0] a ledPanelRgb[2][15] - Informação referente aos leds vermelhos
    // ledPanelRgb[2][16] a ledPanelRgb[2][31] - Informação referente aos leds verdes
    // ledPanelRgb[2][32] a ledPanelRgb[2][47] - Informação referente aos leds Azuis

    // Cada linha possui 64 bits de endereço, cada um referente a um led do painel
    // Obs. Todo esse código foi personalizado para funcionar em um display 16x64, para alterar, é necessário mexer no código fonte para devidas adaptações
    // A informação é dividida nos 4 paineis de 16x16, que juntos formam o 16x64
    //  Display 1   Display 2   Display 3   Display 4
    //  0 a 15      16 a 31     32 a 47     48 a 63
    //  PS. Esse arranjo é para cada linha da variável ledPanelRgb[2][];

    // -- Carrega os espaços de memória dos shift registers para as linhas pares serem exibidas --
    desativaTudo(); // Configura nível lógico alto em A e B, de modo que nenhum led esteja iniciado no display
    toggleOE();
    // delay(1000);
    // ledPanelRgb[painel][linhas] // Essa é a variável que eu tenho que manipular, ela possui 2 linhas de 48 colunas, todas de 64bits

    for (int cores = 2; cores >= 0; cores--) // quando for dois, o valor de adição é 2*16 + ValorAtual = azul e assim sucessivamente para as outras duas cores
    {
        for (int painel = 0; painel <= 1; painel++)
        {
            if (!painel)
            {
                for (int displayID = 0; displayID <= 3; displayID++) // Isso é a indicação de 4 displays
                {
                    for (int linhas = (0 + cores * 16); linhas <= (15 + cores * 16); linhas += 2) // Vai descarregar as linhas ímpares de cada painel, 8 linhas por vez
                    // Ou seja, essa função tem que ser executada 4 vezes para varrer uma das cores
                    {
                        // para que a informação seja enviada para o painel, está sendo utilizado o periférico de SPI do microcontrolador
                        // Porem a informação do painel é uma variável de 64 bits, tendo que ser repartida 4 vezes para ser enviada para os paineis
                        uint16_t slicedInfo = (ledPanelRgb[painel][linhas] >> 16 * displayID); // O right shift serve para saber qual pedado de ledPanelRgb[2][] deve ser enviado naquele momento
                        SPI.beginTransaction(SPISettings(4000000, LSBFIRST, SPI_MODE0));
                        
                        SPI.transfer16(slicedInfo); // Responsável por enviar a informação para os shiftregisters do painel
                        
                        SPI.endTransaction();
                        /* code */
                    }
                }
            }
            else
            {
                // for (int qnt = 0; qnt < 32; qnt++)
                // {
                // 
                // SPI.transfer16(0); // Responsável por enviar a informação para os shiftregisters do painel
                // 
                for (int displayID = 3; displayID >= 0; displayID--) // Isso é a indicação de 4 displays
                {
                    for (int linhas = (15 + cores * 16); linhas >= (cores * 16); linhas -= 2) // Vai descarregar as linhas ímpares de cada painel, 8 linhas por vez
                    // Ou seja, essa função tem que ser executada 4 vezes para varrer uma das cores
                    {
                        // para que a informação seja enviada para o painel, está sendo utilizado o periférico de SPI do microcontrolador
                        // Porem a informação do painel é uma variável de 64 bits, tendo que ser repartida 4 vezes para ser enviada para os paineis
                        uint16_t slicedInfo = (ledPanelRgb[painel][linhas] >> 16 * displayID); // O right shift serve para saber qual pedado de ledPanelRgb[2][] deve ser enviado naquele momento
                        SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
                        
                        SPI.transfer16(slicedInfo); // Responsável por enviar a informação para os shiftregisters do painel
                        
                        SPI.endTransaction();
                        /* code */
                    }
                }
            }
        }
    }
    latchPanel();    // Manda sinal de Latch para os shiftRegisters liberando a saída das informações para as memórias
    ativaLinhaPar(); // Ativa para que as linhas pares sejam ativas e a informação mostrada
    toggleOE();
    delay(1); // Delay necessário para que os nossos olhos percebam a ativação do painel de forma devida
    // -- Carrega os espaços de memória dos shift registers para as linhas impares serem exibidas --

    desativaTudo();
    toggleOE();
    // delay(1000);
    for (int cores = 2; cores >= 0; cores--) // quando for dois, o valor de adição é 2*16 + ValorAtual = azul e assim sucessivamente para as outras duas cores
    {
        for (int painel = 0; painel <= 1; painel++)
        {
            if (!painel)
            {
                for (int displayID = 0; displayID <= 3; displayID++) // Isso é a indicação de 4 displays
                {
                    for (int linhas = (1 + cores * 16); linhas <= (15 + cores * 16); linhas += 2) // Vai descarregar as linhas ímpares de cada painel, 8 linhas por vez
                    // Ou seja, essa função tem que ser executada 4 vezes para varrer uma das cores
                    {
                        // para que a informação seja enviada para o painel, está sendo utilizado o periférico de SPI do microcontrolador
                        // Porem a informação do painel é uma variável de 64 bits, tendo que ser repartida 4 vezes para ser enviada para os paineis
                        uint16_t slicedInfo = (ledPanelRgb[painel][linhas] >> 16 * displayID); // O right shift serve para saber qual pedado de ledPanelRgb[2][] deve ser enviado naquele momento
                        SPI.beginTransaction(SPISettings(4000000, LSBFIRST, SPI_MODE0));
                        
                        SPI.transfer16(slicedInfo); // Responsável por enviar a informação para os shiftregisters do painel
                        
                        SPI.endTransaction();
                        /* code */
                    }
                }
            }
            else
            {
                // for (int qnt = 0; qnt < 32; qnt++)
                // {
                // 
                // SPI.transfer16(0); // Responsável por enviar a informação para os shiftregisters do painel
                // 
                for (int displayID = 3; displayID >= 0; displayID--) // Isso é a indicação de 4 displays
                {
                    for (int linhas = (14 + cores * 16); linhas >= (cores * 16); linhas -= 2) // Vai descarregar as linhas ímpares de cada painel, 8 linhas por vez
                    // Ou seja, essa função tem que ser executada 4 vezes para varrer uma das cores
                    {
                        // para que a informação seja enviada para o painel, está sendo utilizado o periférico de SPI do microcontrolador
                        // Porem a informação do painel é uma variável de 64 bits, tendo que ser repartida 4 vezes para ser enviada para os paineis
                        uint16_t slicedInfo = (ledPanelRgb[painel][linhas] >> 16 * displayID); // O right shift serve para saber qual pedado de ledPanelRgb[2][] deve ser enviado naquele momento
                        SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
                        
                        SPI.transfer16(slicedInfo); // Responsável por enviar a informação para os shiftregisters do painel
                        
                        SPI.endTransaction();
                        /* code */
                    }
                }
            }
        }
    }

    // for (int cores = 2; cores >= 0; cores--) // quando for dois, o valor de adição é 2*16 + ValorAtual = azul e assim sucessivamente para as outras duas cores
    // {
    //     for (int painel = 1; painel >= 0; painel--)
    //     {
    //         for (int displayID = 3; displayID >= 0; displayID--) // Isso é a indicação de 4 displays
    //         {
    //             for (int linhas = (14 + cores * 16); linhas >= (cores * 16); linhas -= 2) // Vai descarregar (0,2,4,6,8,10,12,14) 8 linhas de cada painel
    //             // Ou seja, essa função tem que ser executada 4 vezes para varrer uma das cores
    //             {
    //                 // para que a informação seja enviada para o painel, está sendo utilizado o periférico de SPI do microcontrolador
    //                 // Porem a informação do painel é uma variável de 64 bits, tendo que ser repartida 4 vezes para ser enviada para os paineis
    //                 uint16_t slicedInfo = (ledPanelRgb[painel][linhas] >> 16 * displayID); // O right shift serve para saber qual pedado de ledPanelRgb[2][] deve ser enviado naquele momento
    //                 
    //                 SPI.transfer16(slicedInfo); // Responsável por enviar a informação para os shiftregisters do painel
    //                 
    //                 /* code */
    //             }
    //         }
    //     }
    // }
    latchPanel();      // Manda sinal de Latch para os shiftRegisters liberando a saída das informações para as memórias
    ativaLinhaImpar(); // Ativa para que as linhas impares sejam ativas e a informação mostrada
    toggleOE();
    delay(1); // Delay necessário para que os nossos olhos percebam a ativação do painel de forma devida

} //  --- Fim da função ---

// =========================
// --- Funções gráficas ----
void putPixelMemory(int x, int y, int color) // Coloca um ponto qualquer na váriável a ser varrida
{                                            // 0 <= x <= 64; 0 <= y <= 16
    uint64_t unidade = 1;
    uint64_t tempar = 63 - x % 64;
    int multiplier = 0;
    switch (color)
    {
    case red:
    {
        multiplier = 0;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
    }
    break;

    case green:
    {
        multiplier = 1;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
    }
    break;

    case blue:
    {
        multiplier = 2;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
    }
    break;

    case yellow:
    {
        multiplier = 0;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
        multiplier = 1;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
    }
    break;

    case cyan:
    {
        multiplier = 1;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
        multiplier = 2;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
    }
    break;

    case purple:
    {
        multiplier = 0;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
        multiplier = 2;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
    }
    break;

    case white:
    {
        multiplier = 0;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
        multiplier = 1;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
        multiplier = 2;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] |= ((unidade << tempar));
    }
    break;
    }
}

void clearPixelMemory(int x, int y, int color) // Limpa um ponto qualquer na váriável a ser varrida
{                                              // 0 <= x <= 64; 0 <= y <= 16
    uint64_t unidade = 1;
    uint64_t tempar = 63 - x % 64;
    int multiplier = 0;
    switch (color)
    {
    case red:
        multiplier = 0;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
        // uint32_t temp1 = ledPanelRgb[2][i]>>32;
        // uint32_t temp2 = ledPanelRgb[2][i];
        // Serial.println(temp1,HEX);
        // Serial.println(temp2,HEX);

        break;
    case green:
        /* code */
        multiplier = 1;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
        break;

    case blue:
        /* code */
        multiplier = 2;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
        break;
    case yellow:
    {
        multiplier = 0;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
        multiplier = 1;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
    }
    break;

    case cyan:
    {
        multiplier = 1;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
        multiplier = 2;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
    }
    break;

    case purple:
    {
        multiplier = 0;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
        multiplier = 2;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
    }
    break;

    case white:
    {
        multiplier = 0;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
        multiplier = 1;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
        multiplier = 2;
        ledPanelRgb[1 - x / 64][y + (multiplier * 16)] &= (~(unidade << tempar));
    }
    break;
    }
}

void drawBoxTeams(int team, const bool ch[], int color) // função para desenhar as caixas dos números
{
    // para a nova atualização eu vou ter que fazer caixas com 42 colunas para cada time

    // Os times serão de 1 a 4, podendo escolher a cor deles entre r, g e b. Por enquanto.
    static byte teste = 0;
    switch (color)
    {
    case red:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 32; y++)
            {
                teste = pgm_read_word_near(ch + (32 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, red);
                }
            }
        }
    }
    break;

    case green:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (32); y++)
            {
                teste = pgm_read_word_near(ch + (32 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, green);
                }
            }
        }
    }
    break;

    case blue:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (32); y++)
            {
                teste = pgm_read_word_near(ch + (32 * x + y)); // Essa função lê o que está escrito na memória de programa
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, blue);
                }
            }
        }
    }
    break;
    case yellow:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (32); y++)
            {
                teste = pgm_read_word_near(ch + (32 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, red);
                }
            }
        }

        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (32); y++)
            {
                teste = pgm_read_word_near(ch + (32 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, green);
                }
            }
        }
    }
    break;
    }
}

void drawBoxTeams42x16(int team, const bool ch[], int color) // função para desenhar as caixas dos números
{
    // para a nova atualização eu vou ter que fazer caixas com 42 colunas para cada time

    // Os times serão de 1 a 4, podendo escolher a cor deles entre r, g e b. Por enquanto.
    static byte teste = 0;
    switch (color)
    {
    case red:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 42; y++)
            {
                teste = pgm_read_word_near(ch + (42 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (42 * team), x, red);
                }
            }
        }
    }
    break;

    case green:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (42); y++)
            {
                teste = pgm_read_word_near(ch + (42 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (42 * team), x, green);
                }
            }
        }
    }
    break;

    case blue:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (42); y++)
            {
                teste = pgm_read_word_near(ch + (42 * x + y)); // Essa função lê o que está escrito na memória de programa
                if (teste == 1)
                {
                    putPixelMemory(y + (42 * team), x, blue);
                }
            }
        }
    }
    break;
    case yellow:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (42); y++)
            {
                teste = pgm_read_word_near(ch + (42 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (42 * team), x, red);
                }
            }
        }

        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (42); y++)
            {
                teste = pgm_read_word_near(ch + (42 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (42 * team), x, green);
                }
            }
        }
    }
    break;
    }
}

void clearBoxTeams(int team, const bool ch[], int color) // função para desenhar as caixas dos números
{
    // Os times serão de 1 a 4, podendo escolher a cor deles entre r, g e b. Por enquanto.
    static byte teste = 0;
    switch (color)
    {
    case red:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 32; y++)
            {
                teste = pgm_read_word_near(ch + (32 * x + y));
                if (teste == 1)
                {
                    clearPixelMemory(y + (32 * team), x, red);
                }
            }
        }
    }
    break;

    case green:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (32); y++)
            {
                teste = pgm_read_word_near(ch + (32 * x + y));
                if (teste == 1)
                {
                    clearPixelMemory(y + (32 * team), x, green);
                }
            }
        }
    }
    break;

    case blue:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (32); y++)
            {
                teste = pgm_read_word_near(ch + (32 * x + y)); // Essa função lê o que está escrito na memória de programa
                if (teste == 1)
                {
                    clearPixelMemory(y + (32 * team), x, blue);
                }
            }
        }
    }
    break;
    case yellow:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (32); y++)
            {
                teste = pgm_read_word_near(ch + (32 * x + y));
                if (teste == 1)
                {
                    clearPixelMemory(y + (32 * team), x, red);
                }
            }
        }

        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = (0); y < (32); y++)
            {
                teste = pgm_read_word_near(ch + (32 * x + y));
                if (teste == 1)
                {
                    clearPixelMemory(y + (32 * team), x, green);
                }
            }
        }
    }
    break;
    }
}

void limpaNumero(int xi, int yi, int color)
{
    static byte teste = 0;
    switch (color)
    {
    case red:
    {
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 5; y++)
            {
                teste = pgm_read_word_near(vassoura + (5 * x + y));
                // if (teste == 1)
                if (!teste) // Essa função deve transpassar o valor de drawing.h para o painel
                {
                    clearPixelMemory(y + xi, x + yi, red);
                }
            }
        }
    }
    break;
    case green:
    {
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 5; y++)
            {
                teste = pgm_read_word_near(vassoura + (5 * x + y));
                // if (teste == 1)
                if (!teste) // Essa função deve transpassar o valor de drawing.h para o painel
                {
                    clearPixelMemory(y + xi, x + yi, green);
                }
            }
        }
    }
    break;
    case blue:
    {
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 5; y++)
            {
                teste = pgm_read_word_near(vassoura + (5 * x + y));
                // if (teste == 1)
                if (!teste) // Essa função deve transpassar o valor de drawing.h para o painel
                {
                    clearPixelMemory(y + xi, x + yi, blue);
                }
            }
        }
    }
    break;
    }
}

void colocaNumero(int xi, int yi, int numero, int color) // Função para a plotagem de números no placar
{
    static byte recebeValordeMemoria = 0;
    switch (color)
    {
    case red:
    {
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 5; y++)
            {
                // recebeValordeMemoria = pgm_read_word_near(digitosAlg[numero] + ([5 * x + y]));
                recebeValordeMemoria = pgm_read_byte(&(digitosAlg[numero][5 * x + y]));
                // Serial.println(recebeValordeMemoria);
                // if (teste == 1)
                if (recebeValordeMemoria) // Essa função deve transpassar o valor de drawing.h para o painel
                {
                    putPixelMemory(y + xi, x + yi, red);
                }
            }
        }
    }
    break;
    case green:
    {
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 5; y++)
            {
                recebeValordeMemoria = pgm_read_byte(&(digitosAlg[numero][5 * x + y]));
                // Serial.println(recebeValordeMemoria);
                // if (teste == 1)
                if (recebeValordeMemoria) // Essa função deve transpassar o valor de frawing.h para o painel
                {
                    putPixelMemory(y + xi, x + yi, green);
                }
            }
        }
    }
    break;
    case blue:
    {
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 5; y++)
            {
                recebeValordeMemoria = pgm_read_byte(&(digitosAlg[numero][5 * x + y]));
                // Serial.println(recebeValordeMemoria);
                // if (teste == 1)
                if (recebeValordeMemoria) // Essa função deve transpassar o valor de frawing.h para o painel
                {
                    putPixelMemory(y + xi, x + yi, blue);
                }
            }
        }
    }
    break;
    }
}

// =======================================
// --- Funções de chamada para o main ---
void SetTheBoxes() // Função para chamar os desenhos no painel
{
    // drawBoxTeams(0, boxTeam512, red);    // Camel
    // drawBoxTeams(1, boxTeam512, green);  // Rabbit
    // drawBoxTeams(2, boxTeam512, blue);   // Bird
    // drawBoxTeams(3, boxTeam512, yellow); // Bear
}

void SetTheBoxesExtra() // Função para chamar os desenhos no painel
{
    drawBoxTeams(1, boxTeam512, red);   // Rabbit
    drawBoxTeams(1, boxTeam512, blue);  // Bird
    drawBoxTeams(2, boxTeam512, red);   // Camel
    drawBoxTeams(2, boxTeam512, blue);  // Bear
    drawBoxTeams(2, boxTeam512, green); // Bear
}

void configPWM(int intensidade)
{
    // Função original usava registradores AVR (TCCR2A).
    // No ESP32, usaremos LEDC no futuro para gerenciar o brilho.
}

void configScore()
{
    // Esses dois são referentes aos valores do primeiro placar
    int digitoReferenteAtual = 0;
    for (size_t i = 0; i < 4; i++)
    {
        digitoReferenteAtual = teamsScore[i];
        colocaDigitoPlacarRGB(3 + 32 * i, 3, (digitoReferenteAtual / 1000));
        digitoReferenteAtual %= 1000;
        colocaDigitoPlacarRGB(10 + 32 * i, 3, (digitoReferenteAtual / 100));
        digitoReferenteAtual %= 100;
        colocaDigitoPlacarRGB(17 + 32 * i, 3, (digitoReferenteAtual / 10));
        digitoReferenteAtual %= 10;
        colocaDigitoPlacarRGB(24 + 32 * i, 3, (digitoReferenteAtual % 10));
    }
}

void colocaDigitoPlacarRGB(int xi, int yi, int numero)
{
    colocaNumero(xi, yi, numero, red);
    colocaNumero(xi, yi, numero, green);
    colocaNumero(xi, yi, numero, blue);
}

void LimpaDigitoPlacarRGB(int xi, int yi)
{
    limpaNumero(xi, yi, red);
    limpaNumero(xi, yi, green);
    limpaNumero(xi, yi, blue);
}

void counterPlacar(int team) // Mostra o placar guardado dentro da variável TeamsScore
{
    static int digitoReferenteAtualizado = 0;
    if (mudancaDoPainelSeis)
    {
        // LimpaDigitoPlacarRGB(64 + 3  + 32 * (team * 2) , 3);
        // LimpaDigitoPlacarRGB(64 + 10 + 32 * (team * 2), 3);
        // LimpaDigitoPlacarRGB(64 + 17 + 32 * (team * 2), 3);
        // LimpaDigitoPlacarRGB(64 + 24 + 32 * (team * 2), 3);

        // LimpaDigitoPlacarRGB(64 + 3  + 32 * (team * 2 + 1) , 3);
        // LimpaDigitoPlacarRGB(64 + 10 + 32 * (team * 2 + 1), 3);
        // LimpaDigitoPlacarRGB(64 + 17 + 32 * (team * 2 + 1), 3);
        // LimpaDigitoPlacarRGB(64 + 24 + 32 * (team * 2 + 1), 3);

        digitoReferenteAtualizado = teamsScore[team * 2];
        colocaDigitoPlacarRGB(64 + 3 + 32 * (0), 3, (digitoReferenteAtualizado / 1000));
        digitoReferenteAtualizado %= 1000;
        colocaDigitoPlacarRGB(64 + 10 + 32 * (0), 3, (digitoReferenteAtualizado / 100));
        digitoReferenteAtualizado %= 100;
        colocaDigitoPlacarRGB(64 + 17 + 32 * (0), 3, (digitoReferenteAtualizado / 10));
        digitoReferenteAtualizado %= 10;
        colocaDigitoPlacarRGB(64 + 24 + 32 * (0), 3, (digitoReferenteAtualizado % 10));

        digitoReferenteAtualizado = teamsScore[(team * 2) + 1];
        colocaDigitoPlacarRGB(64 + 2 + (32 * 1 + 1), 3, (digitoReferenteAtualizado / 1000));
        digitoReferenteAtualizado %= 1000;
        colocaDigitoPlacarRGB(64 + 9 + (32 * 1 + 1), 3, (digitoReferenteAtualizado / 100));
        digitoReferenteAtualizado %= 100;
        colocaDigitoPlacarRGB(64 + 16 + (32 * 1 + 1), 3, (digitoReferenteAtualizado / 10));
        digitoReferenteAtualizado %= 10;
        colocaDigitoPlacarRGB(64 + 23 + (32 * 1 + 1), 3, (digitoReferenteAtualizado % 10));
    }
}

void alteraPontosTeam(uint8_t team)
{
    static int pontoDecorrido[6] = {0, 0, 0, 0, 0, 0};
    if (flagPonto[team] == 0)
        return;

    if (mudancaDoPainelSeis)
    {
        if (flagCounter) // Essa flag indica se o contador deverá adicionar ou diminuir pontos
        {
            teamsScore[team] += 50 * flagQuantify;
            counterPlacar(team);

            // if((teamsScore[team] > (pontoDecorrido[team] + ((50*flagQuantify)-1))))//||(millis() - millisTarefa1) > 5000)
            // {
            //     pontoDecorrido[team] = teamsScore[team];
            // }
            flagPonto[team] = 0;
        }
        else
        {
            teamsScore[team] -= 50 * flagQuantify;
            counterPlacar(team);

            // if((teamsScore[team] < (pontoDecorrido[team] - ((50*flagQuantify)-1))))//||(millis() - millisTarefa1) > 5000)
            // {
            //     pontoDecorrido[team] = teamsScore[team];
            // }
            flagPonto[team] = 0;
        }
    }
    else
    {
        if (team == 0 || team == 3)
        {

            Serial.println("barrei no if");
            return;
        }
        Serial.println("Passei do if");

        if (flagCounter) // Essa flag indica se o contador deverá adicionar ou diminuir pontos
        {
            teamsScore[team + 3] += 50 * flagQuantify;
            counterPlacar(team);

            // if((teamsScore[team+3] > (pontoDecorrido[team+3] + ((50*flagQuantify)-1))))//||(millis() - millisTarefa1) > 5000)
            // {
            //     pontoDecorrido[team+3] = teamsScore[team+3];
            // }
            flagPonto[team] = 0;
        }
        else
        {
            teamsScore[team + 3] -= 50 * flagQuantify;
            counterPlacar(team);

            // if((teamsScore[team+3] < (pontoDecorrido[team+3] - ((50*flagQuantify)-1))))//||(millis() - millisTarefa1) > 5000)
            // {
            //     pontoDecorrido[team+3] = teamsScore[team+3];
            // }
            flagPonto[team] = 0;
        }
    }
}

void changeFlagState(uint8_t team, uint8_t quantity) // Função que simboliza que tipo de contagem deverá ocorrer de acordo com o botão que foi pressionado
{
    flagPonto[team] = true;
    flagQuantify = quantity;
}

void changeFlagCounter()
{
    flagCounter = !flagCounter;
}

void cleanShiftRegisters() // Limpa todos os shiftregisters antes da função update panel ser iniciada
{
    for (size_t i = 0; i < 192; i++)
    {
        /* code */
        
        SPI.transfer16(0x0);
        
        latchPanel();
    }
}

void fillShiftRegisters() // Preenche todos os shift registers, mas não tem função alguma pois a função update panel sempre atualiza tudo
{
    for (size_t i = 0; i < 192; i++)
    {
        
        SPI.transfer16(~0);
        
        latchPanel();
    }
}

void callBox(int team, bool flagRed, bool flagGreen, bool flagBlue, bool flagYellow) // Função do menu de seleção/Inverte o estado das caixas na hora da seleção
{
    static bool flag = 1;
    if (flag)
    {
        if (flagRed)
            drawBoxTeams(team, boxTeam512, red);
        if (flagGreen)
            drawBoxTeams(team, boxTeam512, green);
        if (flagBlue)
            drawBoxTeams(team, boxTeam512, blue);
        // if(flagYellow) drawBoxTeams(team, boxTeam512, yellow);
        flag = !flag;
    }
    else
    {
        clearBoxTeams(team, boxTeam512, red);
        clearBoxTeams(team, boxTeam512, green);
        clearBoxTeams(team, boxTeam512, blue);
        // clearBoxTeams(team, boxTeam512, yellow);

        flag = !flag;
    }
}

void plotMinistry(int team) // Função do menu de seleção/Inverte o estado das caixas na hora da seleção
{
    // clearMinistryBox();
    switch (team)
    {
    case red:
    {
        colocaLetra(86 + 11 * 0, 2, letraU, white);
        colocaLetra(86 + 11 * 1, 2, letraP, white);
        // colocaLetra(5 + 11*2, 3, letraA, red);
        // colocaLetra(5 + 11*3, 3, letraM, red);
        // colocaLetra(5 + 11*4, 3, letraP, red);

        // colocaLetra(5 + 11*0, 3, letraA, green);
        // colocaLetra(5 + 11*1, 3, letraC, green);
        // colocaLetra(5 + 11*2, 3, letraA, green);
        // colocaLetra(5 + 11*3, 3, letraM, green);
        // colocaLetra(5 + 11*4, 3, letraP, green);

        // colocaLetra(5 + 11*0, 3, letraA, blue);
        // colocaLetra(5 + 11*1, 3, letraC, blue);
        // colocaLetra(5 + 11*2, 3, letraA, blue);
        // colocaLetra(5 + 11*3, 3, letraM, blue);
        // colocaLetra(5 + 11*4, 3, letraP, blue);
    }
    break;

    case green:
    {
        colocaLetra(74 + 11 * 0, 2, letraM, white);
        colocaLetra(74 + 11 * 1, 2, letraO, white);
        colocaLetra(74 + 11 * 2, 2, letraV, white);
        colocaLetra(74 + 11 * 3, 2, letraE, white);
    }
    break;

    case yellow:
    {
        colocaLetra(79 + 11 * 0, 2, letraJ, white);
        colocaLetra(79 + 11 * 1, 2, letraD, white);
        colocaLetra(79 + 11 * 2, 2, letraL, white);
    }
    break;

    case blue:
        colocaLetra(69 + 11 * 0, 2, letraF, white);
        colocaLetra(69 + 11 * 1, 2, letraA, white);
        colocaLetra(69 + 11 * 2, 2, letraR, white);
        colocaLetra(69 + 11 * 3, 2, letraO, white);
        colocaLetra(69 + 11 * 4, 2, letraL, white);
        {
        }
        break;

    default:
        break;
    }
}

void clearMinistryBox()
{
    for (size_t j = 1; j < 15; j++)
    {
        for (size_t i = 64; i < 127; i++)
        {
            clearPixelMemory(i, j, white);
        }
        /* code */
    }
}

void defineBox(int team, bool flagRed, bool flagGreen, bool flagBlue, bool flagYellow)
{
    if (flagRed)
        drawBoxTeams(team, boxTeam512, red);
    if (flagGreen)
        drawBoxTeams(team, boxTeam512, green);
    if (flagBlue)
        drawBoxTeams(team, boxTeam512, blue);
    if (flagYellow)
        drawBoxTeams(team, boxTeam512, yellow);
}

void initializerPanel(int color) // Função para desenhar um caixote branco em todo o painel
{
    for (int i = 0; i < 16; i++)
    {
        putPixelMemory(0, i, color);
        putPixelMemory(127, i, color);
        putPixelMemory(63, i, color);
        putPixelMemory(64, i, color);
    }

    for (int i = 0; i < 64; i++)
    {
        putPixelMemory(i, 0, color);
        putPixelMemory(64 + i, 15, color);
    }

    plotMinistry(0);
}

void toggleOutputEnable() //
{
    static bool prevFlag = 0; // Inicializa a flag

    if (!prevFlag)
    {
        analogWrite(5, 255);
    }
    else
    {
        analogWrite(5, 254);
    }

    prevFlag = !prevFlag; // Muda o estado da flag
}

void maxPwmOutput() // Função para ligar e desligar o painel
{
    static bool prevFlag = 0; // Inicializa a flag

    if (!prevFlag)
    {
        analogWrite(5, 0);
    }
    else
    {
        analogWrite(5, 254);
    }

    prevFlag = !prevFlag; // Muda o estado da flag
}

void drawBoxMove(int team, const bool ch[], int color) // Função para plotar caixas 16*64
{
    // Ps, o variável team não é necessária, farei a alteração depois, mas por enquanto ela é apenas legacy
    static byte teste = 0; // Variável necessária para armazedar os dados obtidos da memória flash
    switch (color)
    {
    case red:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (64 * team), x, red);
                }
            }
        }
    }
    break;

    case green:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, green);
                }
            }
        }
    }
    break;

    case blue:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, blue);
                }
            }
        }
    }
    break;

    case yellow:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, red);
                }
            }
        }

        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, green);
                }
            }
        }
    }
    break;

    case cyan:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, green);
                }
            }
        }

        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, blue);
                }
            }
        }
    }
    break;

    case purple:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, red);
                }
            }
        }

        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, blue);
                }
            }
        }
    }
    break;

    case white:
    {
        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, red);
                }
            }
        }

        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, blue);
                }
            }
        }

        for (size_t x = 0; x < 16; x++)
        {
            for (size_t y = 0; y < 64; y++)
            {
                teste = pgm_read_word_near(ch + (64 * x + y));
                if (teste == 1)
                {
                    putPixelMemory(y + (32 * team), x, green);
                }
            }
        }
    }
    break;
    }

} // Fim da função drawBoxMove

void drawBoxMovePanel(int color) // Está basicamente sem necessidade de usar agora, pois foi feita a função do alfabeto
{
    drawBoxMove(0, initialText, color); // função para desenhar as caixas dos números
    drawBoxMove(2, initialText, color); // função para desenhar as caixas dos números
} // Fim de drawBoxMovePanel

void colocaLetra(int yi, int xi, int letra, int color) // Função para a plotagem de números no placar
{
    static byte recebeValordeMemoria = 0;
    switch (color)
    {
    case red:
    {
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 10; y++)
            {
                // recebeValordeMemoria = pgm_read_word_near(digitosAlg[numero] + ([5 * x + y]));
                recebeValordeMemoria = pgm_read_byte(&(alfabeto[letra][10 * x + y]));
                // Serial.println(recebeValordeMemoria);
                // if (teste == 1)
                if (recebeValordeMemoria) // Essa função deve transpassar o valor de frawing.h para o painel
                {
                    putPixelMemory(y + yi, x + xi, red);
                }
            }
        }
    }
    break;
    case green:
    {
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 10; y++)
            {
                recebeValordeMemoria = pgm_read_byte(&(alfabeto[letra][10 * x + y]));
                // Serial.println(recebeValordeMemoria);
                // if (teste == 1)
                if (recebeValordeMemoria) // Essa função deve transpassar o valor de frawing.h para o painel
                {
                    putPixelMemory(y + yi, x + xi, green);
                }
            }
        }
    }
    break;
    case blue:
    {
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 10; y++)
            {
                recebeValordeMemoria = pgm_read_byte(&(alfabeto[letra][10 * x + y]));
                // Serial.println(recebeValordeMemoria);
                // if (teste == 1)
                if (recebeValordeMemoria) // Essa função deve transpassar o valor de frawing.h para o painel
                {
                    putPixelMemory(y + yi, x + xi, blue);
                }
            }
        }
    }
    break;

    case white:
    {
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 10; y++)
            {
                recebeValordeMemoria = pgm_read_byte(&(alfabeto[letra][10 * x + y]));
                // Serial.println(recebeValordeMemoria);
                // if (teste == 1)
                if (recebeValordeMemoria) // Essa função deve transpassar o valor de frawing.h para o painel
                {
                    putPixelMemory(y + yi, x + xi, red);
                }
            }
        }
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 10; y++)
            {
                recebeValordeMemoria = pgm_read_byte(&(alfabeto[letra][10 * x + y]));
                // Serial.println(recebeValordeMemoria);
                // if (teste == 1)
                if (recebeValordeMemoria) // Essa função deve transpassar o valor de frawing.h para o painel
                {
                    putPixelMemory(y + yi, x + xi, green);
                }
            }
        }
        for (size_t x = 0; x < 10; x++)
        {
            for (size_t y = 0; y < 10; y++)
            {
                recebeValordeMemoria = pgm_read_byte(&(alfabeto[letra][10 * x + y]));
                // Serial.println(recebeValordeMemoria);
                // if (teste == 1)
                if (recebeValordeMemoria) // Essa função deve transpassar o valor de frawing.h para o painel
                {
                    putPixelMemory(y + yi, x + xi, blue);
                }
            }
        }
    }
    break;
    }
}

void seeWhoWon()
{
    int winnerTeam = findMax(teamsScore[0], teamsScore[1], teamsScore[2], teamsScore[3]);
    clearPanelByPixel(); // Limpa todo o painel

    initializerPanel(white); // Função para desenhar um caixote branco em todo o painel

    colocaLetra(10 + 11 * 0, 3, letraT, red);
    colocaLetra(10 + 11 * 1, 3, letraE, red);
    colocaLetra(10 + 11 * 2, 3, letraA, red);
    colocaLetra(10 + 11 * 3, 3, letraM, red);

    colocaLetra(10 + 11 * 0, 3, letraT, blue);
    colocaLetra(10 + 11 * 1, 3, letraE, blue);
    colocaLetra(10 + 11 * 2, 3, letraA, blue);
    colocaLetra(10 + 11 * 3, 3, letraM, blue);

    colocaLetra(10 + 11 * 0, 3, letraT, green);
    colocaLetra(10 + 11 * 1, 3, letraE, green);
    colocaLetra(10 + 11 * 2, 3, letraA, green);
    colocaLetra(10 + 11 * 3, 3, letraM, green);

    colocaNumero(10 + 64 * 1 + 11 * 0, 3, winnerTeam + 1, red);
    colocaLetra(12 + 64 * 1 + 11 * 1, 3, letraW, red);
    colocaLetra(12 + 64 * 1 + 11 * 2, 3, letraO, red);
    colocaLetra(12 + 64 * 1 + 11 * 3, 3, letraN, red);

    colocaNumero(10 + 64 * 1 + 11 * 0, 3, winnerTeam + 1, blue);
    colocaLetra(12 + 64 * 1 + 11 * 1, 3, letraW, blue);
    colocaLetra(12 + 64 * 1 + 11 * 2, 3, letraO, blue);
    colocaLetra(12 + 64 * 1 + 11 * 3, 3, letraN, blue);

    colocaNumero(10 + 64 * 1 + 11 * 0, 3, winnerTeam + 1, green);
    colocaLetra(12 + 64 * 1 + 11 * 1, 3, letraW, green);
    colocaLetra(12 + 64 * 1 + 11 * 2, 3, letraO, green);
    colocaLetra(12 + 64 * 1 + 11 * 3, 3, letraN, green);
}

int findMax(int a, int b, int c, int d)
{
    int max_ab = (a > b) ? a : b;
    int max_cd = (c > d) ? c : d;
    int pointsOfTeamWinner = (max_ab > max_cd) ? max_ab : max_cd;

    if (a == pointsOfTeamWinner)
        return 0;
    if (b == pointsOfTeamWinner)
        return 1;
    if (c == pointsOfTeamWinner)
        return 2;
    if (d == pointsOfTeamWinner)
        return 3;

    return 15;
}

void checaATrocaDePaineis()
{
    if (millis() - tempoParaTrocaDoPlacar >= 5000)
    {
        mudancaDoPainelSeis = !mudancaDoPainelSeis;
        Serial.println("Vou comecar a mostrar um dos valores");
        Serial.print("O valor em questao e: ");
        Serial.println(mudancaDoPainelSeis);

        if (mudancaDoPainelSeis)
        {
            // Função cores dos times primários
            clearPanelByPixel();
            SetTheBoxes();
            counterPlacar(0);
            counterPlacar(1);
            counterPlacar(2);
            counterPlacar(3);
            // Serial.println("Passei no if sim");
        }
        else
        {
            // Função cores dos outros times
            //  aqui a variável mudancaDoPainelSeis = 0
            //  LION
            //  Camel
            clearPanelByPixel();
            SetTheBoxesExtra();
            counterPlacar(1);
            counterPlacar(2);
            // Serial.println("Passei no if nao");
        }

        // Serial.println(mudancaDoPainelSeis);
        tempoParaTrocaDoPlacar = millis();
    }
}

void readPlacarEEPROM()
{
    uint8_t lowBytePart;
    uint8_t highBytePart;

    for (int i = 0; i < 6; i++)
    {
        lowBytePart = EEPROM.read(i * 2);
        highBytePart = EEPROM.read((i * 2) + 1);
        teamsScore[i] = lowBytePart + highBytePart * 256;
    }
}

void savePlacarEEPROM()
{
    uint8_t lowBytePart;
    uint8_t highBytePart;

    for (int i = 0; i < 6; i++)
    {
        lowBytePart = teamsScore[i] & 0b11111111;
        highBytePart = (teamsScore[i] >> 8) & 0b11111111;

        EEPROM.write(i * 2, lowBytePart);
        EEPROM.write((i * 2) + 1, highBytePart);
    }
}

void clearPlacarEEPROM()
{
    for (int i = 0; i < 12; i++)
    {
        EEPROM.write(i, 0);
    }

    for (int i = 0; i < 6; i++)
    {
        teamsScore[i] = 0;
    }
    savePlacarEEPROM(); // <- garante que o zero foi salvo antes do reset
}

void adicionaPontosEquipes(int time)
{
    if (teamsScore[time / 3] < (10000 - (50 + 50 * (time % 3))))
        teamsScore[time / 3] = teamsScore[time / 3] + (50 + 50 * (time % 3));
}
void subtraiPontosEquipes(int time)
{
    if (teamsScore[time / 3] >= (50 + 50 * (time % 3)))
        teamsScore[time / 3] = teamsScore[time / 3] - (50 + 50 * (time % 3));
}
// void (*funcReset)() = 0;

void PlotLinha()
{
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    
    SPI.transfer16(0xFFFF); // Responsável por enviar a informação para os shiftregisters do painel
    
    SPI.endTransaction();

    latchPanel();
}

void ZeroData()
{
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    
    for (int i = 0; i < 8 * 8 * 3; i++)
        SPI.transfer16(0x0000); // Responsável por enviar a informação para os shiftregisters do painel
    
    SPI.endTransaction();

    latchPanel();
}

void ComandosSerial()
{
    static bool manualEnviado = false;

    if (!manualEnviado)
    {
        Serial.println(F("=== COMANDOS DISPONÍVEIS ==="));
        Serial.println(F("A ON / A OFF        → Ativa ou desativa o pino A"));
        Serial.println(F("B ON / B OFF        → Ativa ou desativa o pino B"));
        Serial.println(F("CLK ON / CLK OFF    → Clock SPI manual"));
        Serial.println(F("MOSI ON / MOSI OFF  → Dados SPI manuais"));
        Serial.println(F("DS ON / DS OFF      → Alterna Data Select"));
        Serial.println(F("OE ON / OE OFF      → Output Enable do painel"));
        Serial.println(F("LAT ON / LAT OFF    → Latch dos dados nos registradores"));
        Serial.println(F("ZERODATA            → Limpa os shift registers (via clearShiftRegisters)"));
        Serial.println(F("PLOTLINHA            → Envia uma linha completa de informacao"));
        Serial.println(F("DUMPBUFFER            → Envia toda a variável pela serial"));

        Serial.println();
        Serial.println(F("Digite um comando e pressione Enter"));
        manualEnviado = true;
    }

    if (Serial.available())
    {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        // Formato do comando: "PIN X" ou "PIN X ON|OFF"
        if (cmd.startsWith("A "))
            digitalWrite(PIN_A, cmd.endsWith("ON") ? HIGH : LOW);
        else if (cmd.startsWith("B "))
            digitalWrite(PIN_B, cmd.endsWith("ON") ? HIGH : LOW);
        else if (cmd.startsWith("CLK"))
            digitalWrite(PIN_CLK, cmd.endsWith("ON") ? HIGH : LOW);
        else if (cmd.startsWith("MOSI") || cmd.startsWith("DS"))
            digitalWrite(PIN_DATA, cmd.endsWith("ON") ? HIGH : LOW);
        else if (cmd.startsWith("OE"))
            digitalWrite(PIN_OE, cmd.endsWith("ON") ? HIGH : LOW);
        else if (cmd.startsWith("LAT"))
            digitalWrite(PIN_LAT, cmd.endsWith("ON") ? HIGH : LOW);
        else if (cmd.startsWith("PLOTLINHA"))
            PlotLinha();
        else if (cmd.startsWith("ZERODATA"))
            ZeroData();

        Serial.println("Comando executado: " + cmd);
    }
}