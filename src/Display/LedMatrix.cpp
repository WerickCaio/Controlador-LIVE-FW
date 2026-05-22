#include "LedMatrix.h"
#include "../Hardware/Hal.h"
#include "Drawing.h"

// Framebuffer: [painel esquerdo/direito][linha * 3 planos de cor]
// ledPanelRgb[halfX][y + plano*32]  onde plano: 0=R, 1=G, 2=B
uint64_t ledPanelRgb[2][96];

// ── Pixel único ───────────────────────────────────────────────────────
void Display_PutPixel(int x, int y, int color) {
    if (x < 0 || x > 127 || y < 0 || y > 31) return;

    uint64_t bit  = (uint64_t)1 << (63 - (x % 64));
    int      half = 1 - x / 64;   // 0=direita (x 64-127), 1=esquerda (x 0-63)

    switch (color) {
        case red:
            ledPanelRgb[half][y]      |= bit; break;
        case green:
            ledPanelRgb[half][y + 32] |= bit; break;
        case blue:
            ledPanelRgb[half][y + 64] |= bit; break;
        case yellow:
            ledPanelRgb[half][y]      |= bit;
            ledPanelRgb[half][y + 32] |= bit; break;
        case cyan:
            ledPanelRgb[half][y + 32] |= bit;
            ledPanelRgb[half][y + 64] |= bit; break;
        case purple:
            ledPanelRgb[half][y]      |= bit;
            ledPanelRgb[half][y + 64] |= bit; break;
        case white:
            ledPanelRgb[half][y]      |= bit;
            ledPanelRgb[half][y + 32] |= bit;
            ledPanelRgb[half][y + 64] |= bit; break;
    }
}

void Display_ClearPixel(int x, int y, int color) {
    if (x < 0 || x > 127 || y < 0 || y > 31) return;

    uint64_t mask = ~((uint64_t)1 << (63 - (x % 64)));
    int      half = 1 - x / 64;

    switch (color) {
        case red:
            ledPanelRgb[half][y]      &= mask; break;
        case green:
            ledPanelRgb[half][y + 32] &= mask; break;
        case blue:
            ledPanelRgb[half][y + 64] &= mask; break;
        case white:
            ledPanelRgb[half][y]      &= mask;
            ledPanelRgb[half][y + 32] &= mask;
            ledPanelRgb[half][y + 64] &= mask; break;
    }
}

void Display_Clear() {
    memset(ledPanelRgb, 0, sizeof(ledPanelRgb));
}

// ── Display_Init: moldura de debug (opcional) ─────────────────────────
// Chame no setup() durante testes para visualizar os limites da tela.
void Display_Init() {
    Display_Clear();
    for (int i = 0; i < 32; i++) {
        Display_PutPixel(0,   i, white);
        Display_PutPixel(127, i, white);
        Display_PutPixel(63,  i, white);
        Display_PutPixel(64,  i, white);
    }
    for (int i = 0; i < 128; i++) {
        Display_PutPixel(i, 0,  white);
        Display_PutPixel(i, 31, white);
    }
}

// ── Modo de teste (scroll de texto) ──────────────────────────────────
void Display_TestPattern(int offset_x) {
    Display_Clear();

    // Fileira de cima (fisicamente de cabeça para baixo no painel)
    Display_DrawChar('C', offset_x +  0, 3, red);
    Display_DrawChar('A', offset_x + 11, 3, red);
    Display_DrawChar('S', offset_x + 22, 3, red);
    Display_DrawChar('A', offset_x + 33, 3, red);

    // Fileira de baixo (posição normal)
    Display_DrawChar('B', offset_x +  0, 19, red);
    Display_DrawChar('A', offset_x + 11, 19, red);
    Display_DrawChar('S', offset_x + 22, 19, red);
    Display_DrawChar('E', offset_x + 33, 19, red);
}

// ── Primitivas de desenho ─────────────────────────────────────────────
void Display_DrawChar(char c, int x, int y, int color) {
    int idx = c - 'A';
    if (idx < 0 || idx > 25) return;
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            if (pgm_read_byte(&alfabeto[idx][10 * i + j]))
                Display_PutPixel(j + x, i + y, color);
}

void Display_DrawDigit(int numero, int x, int y, int color) {
    if (numero < 0 || numero > 9) return;
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 5; j++)
            if (pgm_read_byte(&digitosAlg[numero][5 * i + j]))
                Display_PutPixel(j + x, i + y, color);
}

void Display_ClearCharArea(int x, int y, int color) {
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 5; j++)
            Display_ClearPixel(j + x, i + y, color);
}

// ======================================================================
//  VARREDURA SPI — TOPOLOGIA EM "SERPENTE" 128×32
//  Topo: painel de cabeça para baixo | Base: painel normal
// ======================================================================
void Display_Update() {
    HAL_DisableLines();

    // ── Linhas pares físicas (A=1, B=0) ──────────────────────────────
    for (int cor = 2; cor >= 0; cor--) {
        // Base-Direita (painel 0, Y par: 16..30)
        for (int id = 0; id <= 3; id++)
            for (int y = 16 + cor*32; y <= 30 + cor*32; y += 2)
                HAL_SpiTransferNormal(ledPanelRgb[0][y] >> (16 * id));

        // Base-Esquerda (painel 1, Y par: 16..30)
        for (int id = 0; id <= 3; id++)
            for (int y = 16 + cor*32; y <= 30 + cor*32; y += 2)
                HAL_SpiTransferNormal(ledPanelRgb[1][y] >> (16 * id));

        // Topo-Esquerda (painel 1 invertido, Y ímpar: 15..1)
        for (int id = 3; id >= 0; id--)
            for (int y = 15 + cor*32; y >= 1 + cor*32; y -= 2)
                HAL_SpiTransferInverted(ledPanelRgb[1][y] >> (16 * id));

        // Topo-Direita (painel 0 invertido, Y ímpar: 15..1)
        for (int id = 3; id >= 0; id--)
            for (int y = 15 + cor*32; y >= 1 + cor*32; y -= 2)
                HAL_SpiTransferInverted(ledPanelRgb[0][y] >> (16 * id));
    }
    HAL_LatchPanel();
    HAL_SetLinesPar();
    HAL_EnableDisplay();
    delay(1);
    HAL_DisableLines();

    // ── Linhas ímpares físicas (A=0, B=1) ────────────────────────────
    for (int cor = 2; cor >= 0; cor--) {
        // Base-Direita (painel 0, Y ímpar: 17..31)
        for (int id = 0; id <= 3; id++)
            for (int y = 17 + cor*32; y <= 31 + cor*32; y += 2)
                HAL_SpiTransferNormal(ledPanelRgb[0][y] >> (16 * id));

        // Base-Esquerda (painel 1, Y ímpar: 17..31)
        for (int id = 0; id <= 3; id++)
            for (int y = 17 + cor*32; y <= 31 + cor*32; y += 2)
                HAL_SpiTransferNormal(ledPanelRgb[1][y] >> (16 * id));

        // Topo-Esquerda (painel 1 invertido, Y par: 14..0)
        for (int id = 3; id >= 0; id--)
            for (int y = 14 + cor*32; y >= 0 + cor*32; y -= 2)
                HAL_SpiTransferInverted(ledPanelRgb[1][y] >> (16 * id));

        // Topo-Direita (painel 0 invertido, Y par: 14..0)
        for (int id = 3; id >= 0; id--)
            for (int y = 14 + cor*32; y >= 0 + cor*32; y -= 2)
                HAL_SpiTransferInverted(ledPanelRgb[0][y] >> (16 * id));
    }
    HAL_LatchPanel();
    HAL_SetLinesImpar();
    HAL_EnableDisplay();
    delay(1);
}
