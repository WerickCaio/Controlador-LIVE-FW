#include "LedMatrix.h"
#include "../Hardware/Hal.h" 
#include "Drawing.h"         
#include "../include/Config.h" // Para as definições de PIN_*

// Novo Framebuffer simplificado: 1 byte por pixel (3 bits usados: RGB)
// 128 colunas (X) x 32 linhas (Y)
uint8_t ledBuffer[128][32];

// Converte enum Colors para máscara RGB (bit 0 = R, bit 1 = G, bit 2 = B)
uint8_t colorToRgb(int color) {
    switch (color) {
        case red:    return 0b001;
        case green:  return 0b010;
        case blue:   return 0b100;
        case yellow: return 0b011;
        case cyan:   return 0b110;
        case purple: return 0b101;
        case white:  return 0b111;
        case black:  return 0b000;
        default:     return 0b000;
    }
}

void Display_PutPixel(int x, int y, int color) { 
    if (x < 0 || x > 127 || y < 0 || y > 31) return; 
    ledBuffer[x][y] |= colorToRgb(color);
}

void Display_ClearPixel(int x, int y, int color) {
    if (x < 0 || x > 127 || y < 0 || y > 31) return;
    
    // Se a cor passada for black, limpa todos os bits
    if (color == black) {
        ledBuffer[x][y] = 0;
    } else {
        ledBuffer[x][y] &= ~colorToRgb(color);
    }
}

void Display_Clear() {
    memset(ledBuffer, 0, sizeof(ledBuffer));
}

void Display_Init() {
    Display_Clear();
    // Borda
    for (int i = 0; i < 32; i++) {
        Display_PutPixel(0, i, white);
        Display_PutPixel(127, i, white);
        Display_PutPixel(63, i, white);
        Display_PutPixel(64, i, white);
    }
    for (int i = 0; i < 128; i++) {
        Display_PutPixel(i, 0, white);
        Display_PutPixel(i, 31, white); 
    }
}

void Display_TestPattern(int offset_x) {
    Display_Clear(); 

    // Desenha na fileira de CIMA (Que está de cabeça para baixo fisicamente, 
    // mas a lógica nova de mapeamento já espelha na memória por você!)
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
// VARREDURA PARALELA VIA GPIO (128x32 - 1/2 Scan)
// ======================================================================
void Display_Update() {
    HAL_DisableLines(); 

    // SCAN 0: Linhas Pares (A=LOW, B=HIGH)
    for (int step = 0; step < 1024; step++) {
        int panel = step / 128;          // 0 a 7
        int y_idx = (step % 128) / 16;   // 0 a 7
        int x_idx = step % 16;           // 0 a 15
        
        // CÁLCULO DAS COORDENADAS LÓGICAS (IMAGE X,Y) QUE CORRESPONDEM A ESSE PULSO DO SHIFT REGISTER
        
        // Fileira de BAIXO: Encadeamento Normal
        int x_b = panel * 16 + x_idx;
        int y_b = 30 - y_idx * 2;
        
        // Fileira de CIMA: Encadeamento Normal (Painel desvirado!)
        int x_t = panel * 16 + x_idx;
        int y_t = 14 - y_idx * 2;
        
        uint8_t upper_color = ledBuffer[x_t][y_t];
        uint8_t lower_color = ledBuffer[x_b][y_b];
        
        // Escreve os bits nas portas (usando digitalWrite por compatibilidade)
        (upper_color & 0b001) ? digitalWrite(PIN_R1, HIGH) : digitalWrite(PIN_R1, LOW);
        (upper_color & 0b010) ? digitalWrite(PIN_G1, HIGH) : digitalWrite(PIN_G1, LOW);
        (upper_color & 0b100) ? digitalWrite(PIN_B1, HIGH) : digitalWrite(PIN_B1, LOW);

        (lower_color & 0b001) ? digitalWrite(PIN_R2, HIGH) : digitalWrite(PIN_R2, LOW);
        (lower_color & 0b010) ? digitalWrite(PIN_G2, HIGH) : digitalWrite(PIN_G2, LOW);
        (lower_color & 0b100) ? digitalWrite(PIN_B2, HIGH) : digitalWrite(PIN_B2, LOW);

        // Pulso de Clock
        digitalWrite(PIN_CLK, HIGH);
        digitalWrite(PIN_CLK, LOW);
    }

    HAL_LatchPanel();
    HAL_SetLinesPar();
    HAL_EnableDisplay();
    delayMicroseconds(1000); // 1ms de brilho 
    HAL_DisableLines();

    // SCAN 1: Linhas Ímpares (A=HIGH, B=LOW)
    for (int step = 0; step < 1024; step++) {
        int panel = step / 128;
        int y_idx = (step % 128) / 16;
        int x_idx = step % 16;
        
        int x_b = panel * 16 + x_idx;
        int y_b = 31 - y_idx * 2;
        
        int x_t = panel * 16 + x_idx;
        int y_t = 15 - y_idx * 2;
        
        uint8_t upper_color = ledBuffer[x_t][y_t];
        uint8_t lower_color = ledBuffer[x_b][y_b];
        
        (upper_color & 0b001) ? digitalWrite(PIN_R1, HIGH) : digitalWrite(PIN_R1, LOW);
        (upper_color & 0b010) ? digitalWrite(PIN_G1, HIGH) : digitalWrite(PIN_G1, LOW);
        (upper_color & 0b100) ? digitalWrite(PIN_B1, HIGH) : digitalWrite(PIN_B1, LOW);

        (lower_color & 0b001) ? digitalWrite(PIN_R2, HIGH) : digitalWrite(PIN_R2, LOW);
        (lower_color & 0b010) ? digitalWrite(PIN_G2, HIGH) : digitalWrite(PIN_G2, LOW);
        (lower_color & 0b100) ? digitalWrite(PIN_B2, HIGH) : digitalWrite(PIN_B2, LOW);

        // Pulso de Clock
        digitalWrite(PIN_CLK, HIGH);
        digitalWrite(PIN_CLK, LOW);
    }

    HAL_LatchPanel();
    HAL_SetLinesImpar();
    HAL_EnableDisplay();
    delayMicroseconds(1000); // 1ms de brilho
    HAL_DisableLines(); // <-- Desliga ao final para evitar chuviscos e fantasmas
}