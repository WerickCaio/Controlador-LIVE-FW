#include <Arduino.h>
#include "Display/LedMatrix.h" // A sua nova camada HAL com DMA

void setup()
{
    Serial.begin(115200);
    delay(3000); // Trava de segurança do USB nativo do S3

    Serial.println("\n=== Iniciando Motor DMA HUB75 ===");

    // 1. Liga o painel (o DMA assume o controle do hardware a partir daqui)
    Display_Init();

    // 2. Limpa qualquer lixo da memória RAM
    Display_Clear();

    // 3. Teste de Bordas (Valida a resolução 64x32)
    // Se a pinagem estiver 100% correta, esses 4 pontos vão acender exatamente nas quinas extremas.
    Display_PutPixel(0, 0, red);      // Canto Superior Esquerdo
    Display_PutPixel(63, 0, green);   // Canto Superior Direito
    Display_PutPixel(0, 31, blue);    // Canto Inferior Esquerdo
    Display_PutPixel(63, 31, yellow); // Canto Inferior Direito

    // 4. Teste de Linha (Uma cruz no meio da tela para testar o endereçamento)
    for (int x = 0; x < 64; x++)
    {
        Display_PutPixel(x, 15, purple); // Linha horizontal cruzando a tela
    }
    for (int y = 0; y < 32; y++)
    {
        Display_PutPixel(31, y, cyan); // Linha vertical cruzando a tela
    }

    Serial.println("Desenho estático enviado para a RAM do painel.");
}

void loop()
{
    // -------------------------------------------------------------
    // A MÁGICA DO DMA ACONTECE AQUI: NADA!
    // -------------------------------------------------------------
    // Ao contrário do código antigo em que você precisava ficar atualizando
    // as linhas pares e ímpares o tempo todo, aqui a CPU está 100% LIVRE.
    // O hardware do S3 está lendo a memória RAM e cuspindo pro painel sozinho
    // a milhares de Hertz de frequência. A tela nunca mais vai piscar.

    delay(1000);
}