#include "PanelServer.h"
#include "../../include/Config.h"
#include <WiFi.h>
#include <WebServer.h>   // Biblioteca do Arduino/ESP32 (sem conflito agora)

// ── Instâncias ────────────────────────────────────────────────────────
static WebServer server(80);
static int comandoAtual = -1;

// ── Credenciais do Access Point ───────────────────────────────────────
// Altere ssid e password conforme necessário.
// Deixe password = "" para rede aberta (mais fácil para testes).
static const char* ssid     = "Placar_Escolar";
static const char* password = "";

// ======================================================================
//  PROTOCOLO DE COMANDOS
//
//    0             → Iniciar jogo / reiniciar
//    1..NUM_TEAMS  → +50 pts ao time (teamID = cmd - 1)
//  101..100+N      → -50 pts do time (teamID = cmd - 101)
//  200             → Zerar placar
//  201             → Salvar placar na EEPROM
// ======================================================================

// Mapeia a constante de cor interna para a classe CSS correspondente
static const char* colorToCss(int color) {
    switch (color) {
        case COR_VERMELHO: return "btn-red";
        case COR_AZUL:     return "btn-blue";
        case COR_VERDE:    return "btn-green";
        case COR_AMARELO:  return "btn-yellow";
        case COR_CIANO:    return "btn-cyan";
        case COR_ROXO:     return "btn-purple";
        default:           return "btn-white";
    }
}

// ── Página principal (gerada dinamicamente com os times configurados) ─
static void handleRoot() {
    String html;
    html.reserve(2048);

    html += F("<!DOCTYPE html><html><head>"
              "<meta name='viewport' content='width=device-width,initial-scale=1'>"
              "<meta charset='UTF-8'>"
              "<style>"
              "body{font-family:Arial,sans-serif;text-align:center;"
              "background:#1a1a2e;color:#eee;padding:12px;max-width:400px;margin:0 auto;}"
              "h2{color:#00d4ff;margin:6px 0 14px;font-size:22px;}"
              "h3{color:#aaa;font-size:13px;margin:14px 0 6px;text-transform:uppercase;}"
              ".grid{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:4px;}"
              ".btn{padding:16px 8px;font-size:17px;font-weight:bold;"
              "border:none;border-radius:10px;cursor:pointer;width:100%;}"
              ".btn-full{width:100%;padding:14px;font-size:16px;font-weight:bold;"
              "border:none;border-radius:10px;cursor:pointer;margin:4px 0;display:block;}"
              ".btn-red{background:#e74c3c;color:#fff;}"
              ".btn-blue{background:#3498db;color:#fff;}"
              ".btn-green{background:#27ae60;color:#fff;}"
              ".btn-yellow{background:#f1c40f;color:#111;}"
              ".btn-cyan{background:#00bcd4;color:#111;}"
              ".btn-purple{background:#9b59b6;color:#fff;}"
              ".btn-white{background:#ecf0f1;color:#111;}"
              ".btn-start{background:#00d4ff;color:#111;font-size:18px;}"
              ".btn-zero{background:#e67e22;color:#fff;}"
              ".btn-save{background:#2ecc71;color:#fff;}"
              "a{text-decoration:none;}"
              "hr{border-color:#333;margin:14px 0;}"
              "</style></head><body>");

    html += F("<h2>&#127942; PLACAR ESCOLAR</h2>");

    // Botão iniciar jogo (cmd 0)
    html += F("<a href='/cmd?id=0'><button class='btn-full btn-start'>&#9654; INICIAR JOGO</button></a>");
    html += F("<hr>");

    // ── Botões de +50 pts ──────────────────────────────────────────
    html += F("<h3>&#10133; Adicionar 50 pontos</h3><div class='grid'>");
    for (int i = 0; i < NUM_TEAMS; i++) {
        html += "<a href='/cmd?id=";
        html += String(i + 1);
        html += "'><button class='btn ";
        html += colorToCss(TIMES[i].color);
        html += "'>";
        html += TIMES[i].name;
        html += "</button></a>";
    }
    html += F("</div>");

    // ── Botões de -50 pts ──────────────────────────────────────────
    html += F("<h3>&#10134; Remover 50 pontos</h3><div class='grid'>");
    for (int i = 0; i < NUM_TEAMS; i++) {
        html += "<a href='/cmd?id=";
        html += String(101 + i);
        html += "'><button class='btn ";
        html += colorToCss(TIMES[i].color);
        html += "'>";
        html += TIMES[i].name;
        html += "</button></a>";
    }
    html += F("</div>");

    html += F("<hr>");

    // ── Controles gerais ───────────────────────────────────────────
    html += F("<a href='/cmd?id=200'><button class='btn-full btn-zero'>&#128260; ZERAR PLACAR</button></a>");
    html += F("<a href='/cmd?id=201'><button class='btn-full btn-save'>&#128190; SALVAR PLACAR</button></a>");

    html += F("</body></html>");

    server.send(200, "text/html", html);
}

// ── Recebe o comando e redireciona de volta para a página ─────────────
static void handleCmd() {
    if (server.hasArg("id")) {
        comandoAtual = server.arg("id").toInt();
    }
    server.sendHeader("Location", "/");
    server.send(303);
}

// ======================================================================
//  FUNÇÕES PÚBLICAS
// ======================================================================

void PanelServer_Init() {
    WiFi.softAP(ssid, password);
    IPAddress ip = WiFi.softAPIP();
    DEBUG_PRINT("[WiFi] AP '");
    DEBUG_PRINT(ssid);
    DEBUG_PRINT("' iniciado. Acesse: http://");
    DEBUG_PRINTLN(ip);

    server.on("/",    handleRoot);
    server.on("/cmd", handleCmd);
    server.begin();
    DEBUG_PRINTLN("[WiFi] Servidor HTTP na porta 80.");
}

int PanelServer_GetCommand() {
    server.handleClient();   // Polling síncrono — não bloqueia o loop
    int tmp = comandoAtual;
    comandoAtual = -1;       // Reseta para não repetir o mesmo comando
    return tmp;
}
