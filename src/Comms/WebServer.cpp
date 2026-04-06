#include "WebServer.h"
#include <WiFi.h>
#include <WebServer.h>

// Instancia o servidor na porta 80 (padrão de navegadores)
WebServer server(80);

// Variável para guardar o comando até o loop principal buscar
int comandoAtual = -1;

// Nome da rede Wi-Fi que o placar vai criar
const char* ssid = "Placar_C3";
const char* password = ""; // Deixe vazio para rede aberta nos testes

// =========================================================
// PÁGINA HTML DO APLICATIVO
// =========================================================
void handleRoot() {
    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>";
    html += "body { font-family: Arial; text-align: center; background-color: #222; color: white; margin-top: 20px; }";
    html += "button { width: 45%; padding: 15px; margin: 5px; font-size: 18px; border-radius: 8px; border: none; cursor: pointer; font-weight: bold; }";
    html += ".btn-start { background-color: #4CAF50; color: white; width: 93%; }";
    html += ".btn-red { background-color: #e74c3c; color: white; }";
    html += ".btn-blue { background-color: #3498db; color: white; }";
    html += ".btn-yellow { background-color: #f1c40f; color: black; }";
    html += ".btn-green { background-color: #2ecc71; color: white; }";
    html += ".btn-zero { background-color: #e67e22; color: white; width: 93%; }";
    html += "a { text-decoration: none; }";
    html += "</style></head><body>";
    
    html += "<h2>CONTROLE DO PLACAR</h2>";
    
    // Comando 0: Sair do teste e iniciar o jogo
    html += "<a href=\"/cmd?id=0\"><button class='btn-start'>INICIAR JOGO</button></a><br><br>";

    html += "<h3>Adicionar Pontos (+50)</h3>";
    html += "<a href=\"/cmd?id=1\"><button class='btn-red'>Time BUS</button></a>";     // Cmd 1 (Time 0)
    html += "<a href=\"/cmd?id=4\"><button class='btn-blue'>Time LAN</button></a><br>"; // Cmd 4 (Time 1)
    
    html += "<a href=\"/cmd?id=7\"><button class='btn-yellow'>Time WOO</button></a>";   // Cmd 7 (Time 2)
    html += "<a href=\"/cmd?id=10\"><button class='btn-green'>Time RAI</button></a><br><br>"; // Cmd 10 (Time 3)

    html += "<h3>Remover Pontos (-50)</h3>";
    html += "<a href=\"/cmd?id=13\"><button class='btn-red'>Time BUS</button></a>";     // Cmd 13 (Time 0)
    html += "<a href=\"/cmd?id=16\"><button class='btn-blue'>Time LAN</button></a><br>"; // Cmd 16 (Time 1)

    // Comando 25: Zerar placar
    html += "<br><a href=\"/cmd?id=25\"><button class='btn-zero'>ZERAR PLACAR</button></a>";

    html += "</body></html>";
    
    server.send(200, "text/html", html);
}

// =========================================================
// RECEPÇÃO DE COMANDOS
// =========================================================
void handleCmd() {
    if (server.hasArg("id")) {
        comandoAtual = server.arg("id").toInt();
    }
    // Redireciona de volta para a página principal para não sair da tela
    server.sendHeader("Location", "/");
    server.send(303);
}

// =========================================================
// FUNÇÕES PRINCIPAIS CHAMADAS PELO MAIN
// =========================================================
void WebServer_Init() {
    // Inicia o ESP32 como um Roteador (Access Point)
    WiFi.softAP(ssid, password);
    
    server.on("/", handleRoot);
    server.on("/cmd", handleCmd);
    server.begin();
}

int WebServer_GetCommand() {
    // Atende clientes Wi-Fi rapidamente (Pooling Síncrono)
    server.handleClient();
    
    int cmdTemporario = comandoAtual;
    comandoAtual = -1; // Reseta a variável após a leitura para não repetir comando
    
    return cmdTemporario;
}