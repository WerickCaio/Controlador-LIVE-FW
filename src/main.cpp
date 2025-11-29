#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// --- Configurações do Ponto de Acesso ---
const char* ssid = "Controlador_LIVE_Werick"; // Nome da Rede
const char* password = "senha_secreta";       // Senha (min 8 chars)

// Instancia o servidor na porta 80 (padrão web)
WebServer server(80);

// --- Hardware ---
#define LED_BUILTIN 2

// --- Handlers do Servidor (Funções que respondem ao celular) ---

void handleRoot() {
  // Quando alguém acessar o IP do ESP32, mostramos isso:
  server.send(200, "text/html", "<h1>Controlador LIVE</h1><p>Conectado com sucesso!</p>");
  
  // Feedback visual: Pisca o LED rápido para indicar acesso
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); 
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Nao encontrado");
}

// --- Setup do Sistema ---

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  // 1. Inicia o SoftAP (Cria a rede)
  Serial.println("\nIniciando Ponto de Acesso...");
  WiFi.softAP(ssid, password);

  // 2. Mostra o IP para conectarmos
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // 3. Define as rotas do servidor
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  // 4. Inicia o servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  // Mantém o servidor escutando requisições
  server.handleClient();
  
  // Aqui poderíamos ter um delay pequeno ou lógica não-bloqueante
}