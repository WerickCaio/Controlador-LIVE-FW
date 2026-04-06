#include "LedMatrix.h"
#include "../Hardware/Hal.h" 
#include "Drawing.h"         

uint64_t ledPanelRgb[2][96]; // Memória dobrada (32 linhas * 3 cores)

void Display_PutPixel(int x, int y, int color) { 
    if (x < 0 || x > 127 || y < 0 || y > 31) return; 

    uint64_t unidade = 1;
    uint64_t tempar = 63 - (x % 64);
    int multiplier = 0;
    
    switch (color) {
        case red:
            multiplier = 0; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar)); break;
        case green:
            multiplier = 1; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar)); break;
        case blue:
            multiplier = 2; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar)); break;
        case yellow:
            multiplier = 0; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar));
            multiplier = 1; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar)); break;
        case cyan:
            multiplier = 1; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar));
            multiplier = 2; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar)); break;
        case purple:
            multiplier = 0; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar));
            multiplier = 2; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar)); break;
        case white:
            multiplier = 0; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar));
            multiplier = 1; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar));
            multiplier = 2; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] |= ((unidade << tempar)); break;
    }
}

void Display_ClearPixel(int x, int y, int color) {
    if (x < 0 || x > 127 || y < 0 || y > 31) return;

    uint64_t unidade = 1;
    uint64_t tempar = 63 - (x % 64);
    int multiplier = 0;
    
    switch (color) {
        case red:
            multiplier = 0; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] &= (~(unidade << tempar)); break;
        case green:
            multiplier = 1; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] &= (~(unidade << tempar)); break;
        case blue:
            multiplier = 2; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] &= (~(unidade << tempar)); break;
        case white:
            multiplier = 0; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] &= (~(unidade << tempar));
            multiplier = 1; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] &= (~(unidade << tempar));
            multiplier = 2; ledPanelRgb[1 - x / 64][y + (multiplier * 32)] &= (~(unidade << tempar)); break;
    }
}

void Display_Clear() {
    // Adeus, fantasmas! O memset "zera" toda a matriz de vídeo instantaneamente.
    memset(ledPanelRgb, 0, sizeof(ledPanelRgb));
}

void Display_Init() {
    Display_Clear();
    for (int i = 0; i < 32; i++) {
        Display_PutPixel(0, i, white);
        Display_PutPixel(127, i, white);
        Display_PutPixel(63, i, white);
        Display_PutPixel(64, i, white);
    }
    for (int i = 0; i < 64; i++) {
        Display_PutPixel(i, 0, white);
        Display_PutPixel(64 + i, 31, white); 
    }
}

void Display_TestPattern(int offset_x) {
    Display_Clear(); 

    // Desenha na fileira de CIMA (Que está de cabeça para baixo fisicamente)
    Display_DrawChar('C', offset_x + 0, 3, red);
    Display_DrawChar('A', offset_x + 11, 3, red);
    Display_DrawChar('S', offset_x + 22, 3, red);
    Display_DrawChar('A', offset_x + 33, 3, red);

    // Desenha na fileira de BAIXO (Que está na posição normal fisicamente)
    Display_DrawChar('B', offset_x + 0, 19, red);
    Display_DrawChar('A', offset_x + 11, 19, red);
    Display_DrawChar('S', offset_x + 22, 19, red);
    Display_DrawChar('E', offset_x + 33, 19, red);
}

void Display_DrawChar(char c, int x, int y, int color) {
    int letra = c - 'A'; 
    if (letra < 0 || letra > 25) return;
    
    for (size_t i = 0; i < 10; i++) {
        for (size_t j = 0; j < 10; j++) {
            if (pgm_read_byte(&(alfabeto[letra][10 * i + j]))) {
                Display_PutPixel(j + x, i + y, color);
            }
        }
    }
}

void Display_DrawDigit(int numero, int x, int y, int color) {
    if(numero < 0 || numero > 9) return;
    
    for (size_t i = 0; i < 10; i++) {
        for (size_t j = 0; j < 5; j++) {
            if (pgm_read_byte(&(digitosAlg[numero][5 * i + j]))) {
                Display_PutPixel(j + x, i + y, color);
            }
        }
    }
}

void Display_ClearCharArea(int x, int y, int color) {
    for (size_t i = 0; i < 10; i++) {
        for (size_t j = 0; j < 5; j++) {
            Display_ClearPixel(j + x, i + y, color);
        }
    }
}

// ======================================================================
// VARREDURA SPI CASCATA 128x32
// ======================================================================
// ======================================================================
// VARREDURA SPI CASCATA 128x32 (TOPOLOGIA EM "SERPENTE")
// Topo: De cabeça para baixo | Base: Normal
// Fim da Linha (Push 1): Base-Direita (Painel 16)
// ======================================================================
void Display_Update() {
    HAL_DisableLines(); 

    // ====================================================
    // --- LINHAS PARES FÍSICAS (A=1, B=0) ---
    // ====================================================
    for (int cores = 2; cores >= 0; cores--) {
        
        // 1. BASE-DIREITA (Normal, painel 0, Y: 16..31)
        for (int displayID = 0; displayID <= 3; displayID++) {
            for (int linhas = 16 + cores * 32; linhas <= 30 + cores * 32; linhas += 2) {
                HAL_SpiTransferNormal((ledPanelRgb[0][linhas] >> (16 * displayID)));
            }
        }

        // 2. BASE-ESQUERDA (Normal, painel 1, Y: 16..31)
        for (int displayID = 0; displayID <= 3; displayID++) {
            for (int linhas = 16 + cores * 32; linhas <= 30 + cores * 32; linhas += 2) {
                HAL_SpiTransferNormal((ledPanelRgb[1][linhas] >> (16 * displayID)));
            }
        }

        // 3. TOPO-ESQUERDA (Invertido, painel 1, Y: 0..15)
        // Como o painel está de cabeça para baixo, as linhas pares físicas mapeiam as linhas ÍMPARES da imagem (de trás pra frente)
        for (int displayID = 3; displayID >= 0; displayID--) {
            for (int linhas = 15 + cores * 32; linhas >= 1 + cores * 32; linhas -= 2) {
                HAL_SpiTransferInverted((ledPanelRgb[1][linhas] >> (16 * displayID)));
            }
        }

        // 4. TOPO-DIREITA (Invertido, painel 0, Y: 0..15)
        for (int displayID = 3; displayID >= 0; displayID--) {
            for (int linhas = 15 + cores * 32; linhas >= 1 + cores * 32; linhas -= 2) {
                HAL_SpiTransferInverted((ledPanelRgb[0][linhas] >> (16 * displayID)));
            }
        }
    }
    HAL_LatchPanel();
    HAL_SetLinesPar();
    HAL_EnableDisplay();
    delay(1); 
    HAL_DisableLines();

    // ====================================================
    // --- LINHAS ÍMPARES FÍSICAS (A=0, B=1) ---
    // ====================================================
    for (int cores = 2; cores >= 0; cores--) {
        
        // 1. BASE-DIREITA (Normal, painel 0, Y: 16..31)
        for (int displayID = 0; displayID <= 3; displayID++) {
            for (int linhas = 17 + cores * 32; linhas <= 31 + cores * 32; linhas += 2) {
                HAL_SpiTransferNormal((ledPanelRgb[0][linhas] >> (16 * displayID)));
            }
        }

        // 2. BASE-ESQUERDA (Normal, painel 1, Y: 16..31)
        for (int displayID = 0; displayID <= 3; displayID++) {
            for (int linhas = 17 + cores * 32; linhas <= 31 + cores * 32; linhas += 2) {
                HAL_SpiTransferNormal((ledPanelRgb[1][linhas] >> (16 * displayID)));
            }
        }

        // 3. TOPO-ESQUERDA (Invertido, painel 1, Y: 0..15)
        // Linhas ímpares físicas mapeiam as linhas PARES da imagem (de trás pra frente)
        for (int displayID = 3; displayID >= 0; displayID--) {
            for (int linhas = 14 + cores * 32; linhas >= 0 + cores * 32; linhas -= 2) {
                HAL_SpiTransferInverted((ledPanelRgb[1][linhas] >> (16 * displayID)));
            }
        }

        // 4. TOPO-DIREITA (Invertido, painel 0, Y: 0..15)
        for (int displayID = 3; displayID >= 0; displayID--) {
            for (int linhas = 14 + cores * 32; linhas >= 0 + cores * 32; linhas -= 2) {
                HAL_SpiTransferInverted((ledPanelRgb[0][linhas] >> (16 * displayID)));
            }
        }
    }
    HAL_LatchPanel();
    HAL_SetLinesImpar();
    HAL_EnableDisplay();
    delay(1); 
}