#include <Arduino.h>

// Os pinos EXATOS da sua placa nova
const int PIN_R1 = 11;
const int PIN_G1 = 12;
const int PIN_B1 = 10;
const int PIN_R2 = 7;
const int PIN_G2 = 15;
const int PIN_B2 = 6;
const int PIN_A = 18;
const int PIN_B = 8;
const int PIN_CLK = 9;
const int PIN_LAT = 46;
const int PIN_OE = 3;

int pixel_atual = 0;
unsigned long ultimoTempo = 0;

void setup()
{
    Serial.begin(115200);

    // Configura todos os seus pinos paralelos como SAÍDA
    const int pinos[] = {PIN_R1, PIN_G1, PIN_B1, PIN_R2, PIN_G2, PIN_B2, PIN_A, PIN_B, PIN_CLK, PIN_LAT, PIN_OE};
    for (int p : pinos)
    {
        pinMode(p, OUTPUT);
        digitalWrite(p, LOW);
    }

    Serial.println("Driver Manual Iniciado. Mapeando a matriz...");
}

// A nossa própria função de varredura paralela!
void atualizaPainel()
{
    // 1/4 Scan: Testa os 4 estados dos pinos A e B (00, 01, 10, 11)
    for (int row = 0; row < 4; row++)
    {

        // Define as linhas (A = bit 0, B = bit 1)
        digitalWrite(PIN_A, row & 0x01);
        digitalWrite(PIN_B, row & 0x02);

        // Como são 2 módulos de 16px (1/4 scan), cada linha inteira precisa de 64 clocks.
        for (int clk = 0; clk < 64; clk++)
        {

            // O DETETIVE: Só acendemos o Vermelho (R1) se estivermos na linha 0
            // e no momento exato do clock que estamos testando agora.
            if (row == 0 && clk == pixel_atual)
            {
                digitalWrite(PIN_R1, HIGH);
            }
            else
            {
                digitalWrite(PIN_R1, LOW);
            }

            // Mantém todas as outras cores apagadas
            digitalWrite(PIN_G1, LOW);
            digitalWrite(PIN_B1, LOW);
            digitalWrite(PIN_R2, LOW);
            digitalWrite(PIN_G2, LOW);
            digitalWrite(PIN_B2, LOW);

            // Bate o Clock (Empurra os 6 bits para dentro do painel)
            digitalWrite(PIN_CLK, HIGH);
            digitalWrite(PIN_CLK, LOW);
        }

        // Trava os dados e exibe na tela (Latch & Output Enable)
        digitalWrite(PIN_OE, HIGH);
        digitalWrite(PIN_LAT, HIGH);
        digitalWrite(PIN_LAT, LOW);
        digitalWrite(PIN_OE, LOW);
    }
}

void loop()
{
    // Roda a atualização o mais rápido possível para o LED não piscar (Falso DMA)
    atualizaPainel();

    // A cada 1 segundo, avança o LED para o próximo pulso de clock
    if (millis() - ultimoTempo > 1000)
    {
        pixel_atual++;

        // Se já passou pelos 64 clocks (os 2 painéis), recomeça.
        if (pixel_atual >= 64)
        {
            pixel_atual = 0;
            Serial.println("--- Recomeçando a varredura ---");
        }

        Serial.print("Pulso de Clock: ");
        Serial.println(pixel_atual);
        ultimoTempo = millis();
    }
}