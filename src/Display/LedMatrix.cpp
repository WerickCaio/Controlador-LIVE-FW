#include "LedMatrix.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Ajuste para a resolução física de apenas 1 painel
#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 2 // Se forem 2 painéis em cascata (total 128x32)

// Ponteiro global para o display DMA
MatrixPanel_I2S_DMA *dma_display = nullptr;

void Display_Init()
{
    // 1. Configuramos os seus pinos exatos da placa customizada
    HUB75_I2S_CFG::i2s_pins _customPins = {
        11, // R1
        12, // G1
        10, // B1
        7,  // R2
        15, // G2
        6,  // B2
        18, // A
        8,  // B
        -1, // C  (⚠️ ALERTA DE HARDWARE ABAIXO)
        -1, // D  (⚠️ ALERTA DE HARDWARE ABAIXO)
        -1, // E
        46, // LAT
        3,  // OE
        9   // CLK
    };

    // 2. Criamos as configurações do painel
    HUB75_I2S_CFG mxconfig(
        PANEL_RES_X,
        PANEL_RES_Y,
        PANEL_CHAIN,
        _customPins);

    // 3. Inicializa o motor DMA
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();
    dma_display->clearScreen();
    dma_display->setBrightness8(100); // 0 a 255
}

void Display_Clear()
{
    dma_display->clearScreen();
}

// Convertendo a sua enumeração antiga de cores para o RGB565 da biblioteca
uint16_t getDMAColor(int color)
{
    switch (color)
    {
    case red:
        return dma_display->color565(255, 0, 0);
    case green:
        return dma_display->color565(0, 255, 0);
    case blue:
        return dma_display->color565(0, 0, 255);
    case yellow:
        return dma_display->color565(255, 255, 0);
    case cyan:
        return dma_display->color565(0, 255, 255);
    case purple:
        return dma_display->color565(255, 0, 255);
    case white:
        return dma_display->color565(255, 255, 255);
    default:
        return 0;
    }
}

void Display_PutPixel(int x, int y, int color)
{
    dma_display->drawPixel(x, y, getDMAColor(color));
}

// --- COMO DESENHAR IMAGENS ---
// A biblioteca herda da Adafruit_GFX, o que permite imprimir matrizes diretas!
void Display_DrawImage(int x, int y, const uint8_t *bitmap, int w, int h, int color)
{
    dma_display->drawBitmap(x, y, bitmap, w, h, getDMAColor(color));
}