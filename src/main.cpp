#include <Arduino.h>

// Definição do pino do LED (No ESP32 DevKit V1 geralmente é o pino 2)
#define LED_BUILTIN 2

void setup() {
  // Configura o pino como saída
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Inicia a Serial para debug básico
  Serial.begin(115200);
  Serial.println("Sistema Iniciado: Status OK (Blink)");
}

void loop() {
  // O coração do sistema: batendo a cada 1 segundo
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}