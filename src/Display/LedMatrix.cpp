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
    for (size_t j = 0; j < 128; j++) {
        for (size_t i = 0; i < 32; i++) { 
            Display_ClearPixel(j, i, white); 
        }
    }
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

void Display_TestPattern() {
    Display_Clear();
    // Painel de Cima (Vermelho)
    for (int x = 0; x < 128; x++) { Display_PutPixel(x, 0, red); Display_PutPixel(x, 15, red); }
    for (int y = 0; y < 16; y++) { Display_PutPixel(0, y, red); Display_PutPixel(127, y, red); }

    // Painel de Baixo (Azul)
    for (int x = 0; x < 128; x++) { Display_PutPixel(x, 16, blue); Display_PutPixel(x, 31, blue); }
    for (int y = 16; y < 32; y++) { Display_PutPixel(0, y, blue); Display_PutPixel(127, y, blue); }
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
void Display_Update() {
    HAL_DisableLines(); 

    // --- LINHAS PARES ---
    for (int cores = 2; cores >= 0; cores--) {
        // Envia os dados do Painel de BAIXO (Linhas 16 a 31)
        for (int painel = 0; painel <= 1; painel++) {
            if (!painel) {
                for (int displayID = 0; displayID <= 3; displayID++) {
                    for (int linhas = (16 + cores * 32); linhas <= (30 + cores * 32); linhas += 2) {
                        HAL_SpiTransferNormal((ledPanelRgb[painel][linhas] >> (16 * displayID)));
                    }
                }
            } else {
                for (int displayID = 3; displayID >= 0; displayID--) {
                    for (int linhas = (31 + cores * 32); linhas >= (17 + cores * 32); linhas -= 2) {
                        HAL_SpiTransferInverted((ledPanelRgb[painel][linhas] >> (16 * displayID)));
                    }
                }
            }
        }
        // Envia os dados do Painel de CIMA (Linhas 0 a 15)
        for (int painel = 0; painel <= 1; painel++) {
            if (!painel) {
                for (int displayID = 0; displayID <= 3; displayID++) {
                    for (int linhas = (0 + cores * 32); linhas <= (14 + cores * 32); linhas += 2) {
                        HAL_SpiTransferNormal((ledPanelRgb[painel][linhas] >> (16 * displayID)));
                    }
                }
            } else {
                for (int displayID = 3; displayID >= 0; displayID--) {
                    for (int linhas = (15 + cores * 32); linhas >= (1 + cores * 32); linhas -= 2) {
                        HAL_SpiTransferInverted((ledPanelRgb[painel][linhas] >> (16 * displayID)));
                    }
                }
            }
        }
    }
    HAL_LatchPanel();
    HAL_SetLinesPar();
    HAL_EnableDisplay();
    delay(1); 
    HAL_DisableLines();

    // --- LINHAS ÍMPARES ---
    for (int cores = 2; cores >= 0; cores--) {
        // Envia os dados do Painel de BAIXO
        for (int painel = 0; painel <= 1; painel++) {
            if (!painel) {
                for (int displayID = 0; displayID <= 3; displayID++) {
                    for (int linhas = (17 + cores * 32); linhas <= (31 + cores * 32); linhas += 2) {
                        HAL_SpiTransferNormal((ledPanelRgb[painel][linhas] >> (16 * displayID)));
                    }
                }
            } else {
                for (int displayID = 3; displayID >= 0; displayID--) {
                    for (int linhas = (30 + cores * 32); linhas >= (16 + cores * 32); linhas -= 2) {
                        HAL_SpiTransferInverted((ledPanelRgb[painel][linhas] >> (16 * displayID)));
                    }
                }
            }
        }
        // Envia os dados do Painel de CIMA
        for (int painel = 0; painel <= 1; painel++) {
            if (!painel) {
                for (int displayID = 0; displayID <= 3; displayID++) {
                    for (int linhas = (1 + cores * 32); linhas <= (15 + cores * 32); linhas += 2) {
                        HAL_SpiTransferNormal((ledPanelRgb[painel][linhas] >> (16 * displayID)));
                    }
                }
            } else {
                for (int displayID = 3; displayID >= 0; displayID--) {
                    for (int linhas = (14 + cores * 32); linhas >= (0 + cores * 32); linhas -= 2) {
                        HAL_SpiTransferInverted((ledPanelRgb[painel][linhas] >> (16 * displayID)));
                    }
                }
            }
        }
    }
    HAL_LatchPanel();
    HAL_SetLinesImpar();
    HAL_EnableDisplay();
    delay(1); 
}