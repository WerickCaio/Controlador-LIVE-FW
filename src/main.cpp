#include <Arduino.h>
#include "BluetoothSerial.h"

/*
 * ARQUITETURA:
 * - O ESP32 cria um dispositivo Bluetooth visível.
 * - O celular se conecta e manda texto.
 * - O ESP32 recebe, processa e mostra no Monitor Serial do PC.
 */

// Objeto de controle do Bluetooth
BluetoothSerial SerialBT;

// Flag para verificar se algo foi recebido
bool mensagemRecebida = false;
String mensagem = "";

void setup() {
  // 1. Inicia a Serial de Debug (Cabo USB)
  Serial.begin(115200);
  
  // 2. Inicia o Bluetooth com o nome do dispositivo
  // Se falhar, avisa no monitor
  if(!SerialBT.begin("Controlador_LIVE_BT")) {
    Serial.println("Erro ao iniciar Bluetooth!");
    while(1); // Trava o sistema se falhar
  }
  
  Serial.println("Bluetooth Iniciado! Pode parear com o celular agora.");
}

void loop() {
  // Verifica se há dados chegando do Celular via Bluetooth
  if (SerialBT.available()) {
    char c = SerialBT.read(); // Lê um caractere por vez
    
    // Se for uma quebra de linha, consideramos o fim da mensagem
    if (c == '\n') {
      mensagemRecebida = true;
    } else if (c != '\r') { // Ignora o caractere de retorno de carro
      mensagem += c;
    }
  }

  // Processa a mensagem completa
  if (mensagemRecebida) {
    Serial.print("Mensagem recebida! : (");
    Serial.print(mensagem);
    Serial.println(")");
    
    // Opcional: Responder ao celular para confirmar
    SerialBT.println("ESP32 recebeu: " + mensagem);
    
    // Limpa para a próxima
    mensagem = "";
    mensagemRecebida = false;
  }
  
  // Pequeno delay para estabilidade
  delay(20);
}