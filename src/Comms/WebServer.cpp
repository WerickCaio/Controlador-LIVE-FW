#include "WebServer.h"
#include <WiFi.h>
#include <WebServer.h>
#include "../Game/Scoreboard.h"
#include "../Display/LedMatrix.h" 

WebServer server(80);

const char* ssid = "ACAMP_VOX";
const char* password = "";
bool flagRedraw = false;

// Helpers to map int colors to strings and vice versa
String getColorName(int c) {
    if (c == red) return "red";
    if (c == green) return "green";
    if (c == blue) return "blue";
    if (c == yellow) return "yellow";
    if (c == cyan) return "cyan";
    if (c == purple) return "purple";
    if (c == white) return "white";
    return "black";
}

int getColorValue(String s) {
    if (s == "red") return red;
    if (s == "green") return green;
    if (s == "blue") return blue;
    if (s == "yellow") return yellow;
    if (s == "cyan") return cyan;
    if (s == "purple") return purple;
    if (s == "white") return white;
    return black;
}

void handleRoot() {
    int num = Scoreboard_GetNumTeams();
    int maxChars = (128 / num) / 8; // 8 pixels per char in 7x10 font
    int bright = Scoreboard_GetBrightness();

    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ACAMP VOX - Controle</title>
    <style>
        body { font-family: 'Segoe UI', Tahoma, sans-serif; background-color: #121212; color: #fff; margin: 0; padding: 0; }
        .tabs { display: flex; background: #1e1e24; }
        .tab { flex: 1; text-align: center; padding: 15px; cursor: pointer; font-size: 1.1rem; font-weight: bold; transition: 0.3s; }
        .tab:hover { background: #2a2a35; }
        .tab.active { border-bottom: 4px solid #ea580c; color: #ea580c; background: #18181f; }
        .content { display: none; padding: 20px; align-items: center; flex-direction: column; }
        .content.active { display: flex; }
        
        .dashboard { display: flex; flex-wrap: wrap; gap: 15px; justify-content: center; max-width: 800px; }
        .team-card { background-color: #1e1e24; padding: 15px; border-radius: 12px; text-align: center; width: 140px; box-shadow: 0 4px 10px rgba(0,0,0,0.5);}
        .team-name { font-size: 1.3rem; font-weight: bold; margin-bottom: 5px; text-transform: uppercase; }
        .score { font-size: 2.5rem; font-weight: bold; margin: 10px 0; }
        
        .btn-group { display: flex; gap: 5px; margin-bottom: 5px; }
        .btn-group button { flex: 1; padding: 10px 2px; border: none; border-radius: 4px; color: white; font-weight: bold; cursor: pointer; font-size: 0.95rem; }
        
        /* Cores Dinamicas para as Cartas (Borda) */
        .color-red { border-top: 6px solid #ef4444; } .color-red .btn-pos { background: #dc2626; } .color-red .btn-neg { background: #991b1b; }
        .color-blue { border-top: 6px solid #3b82f6; } .color-blue .btn-pos { background: #2563eb; } .color-blue .btn-neg { background: #1e3a8a; }
        .color-green { border-top: 6px solid #22c55e; } .color-green .btn-pos { background: #16a34a; } .color-green .btn-neg { background: #14532d; }
        .color-yellow { border-top: 6px solid #eab308; } .color-yellow .btn-pos { background: #ca8a04; } .color-yellow .btn-neg { background: #854d0e; }
        .color-cyan { border-top: 6px solid #06b6d4; } .color-cyan .btn-pos { background: #0891b2; } .color-cyan .btn-neg { background: #164e63; }
        .color-purple { border-top: 6px solid #a855f7; } .color-purple .btn-pos { background: #9333ea; } .color-purple .btn-neg { background: #581c87; }
        .color-white { border-top: 6px solid #ffffff; } .color-white .btn-pos { background: #d4d4d8; color:#000; } .color-white .btn-neg { background: #52525b; }

        .log-box { margin-top: 30px; width: 100%; max-width: 600px; background: #1e1e24; border-radius: 8px; padding: 10px; height: 180px; overflow-y: auto; font-family: monospace; font-size: 0.95rem; color: #aaa; border-left: 4px solid #ea580c; box-sizing: border-box;}
        .log-line { border-bottom: 1px solid #333; padding: 8px 0; }
        
        .config-list { display: flex; flex-direction: column; gap: 15px; width: 100%; max-width: 400px; }
        .config-item { display: flex; align-items: center; justify-content: space-between; background: #1e1e24; padding: 15px; border-radius: 8px; flex-wrap:wrap;}
        .config-item input[type='text'], .config-item select { width: 100px; padding: 8px; font-weight: bold; text-align: center; text-transform: uppercase; font-size: 1.1rem; border: none; border-radius: 4px; background:#121212; color:white; margin-top:5px; }
        .config-item input[type='range'] { width: 100%; margin-top: 10px; }
        
        .btn-save { background: #ea580c; border: none; color: white; padding: 15px 20px; font-size: 1.1rem; font-weight: bold; border-radius: 6px; cursor: pointer; margin-top: 20px; width: 100%; max-width: 400px;}
        .btn-reset { background: #991b1b; border: none; color: white; padding: 15px 20px; font-size: 1.1rem; font-weight: bold; border-radius: 6px; cursor: pointer; margin-top: 40px; width: 100%; max-width: 400px;}
    </style>
</head>
<body>
    <div class="tabs">
        <div class="tab active" onclick="switchTab('placar', this)">🎮 Placar</div>
        <div class="tab" onclick="switchTab('config', this)">⚙️ Configurações</div>
        <div class="tab" onclick="switchTab('diag', this)">🛠️ Diagnóstico</div>
    </div>

    <!-- TELA DO PLACAR -->
    <div id="placar" class="content active">
        <div class="dashboard">
)rawliteral";

    for (int i = 0; i < num; i++) {
        String colorStr = getColorName(Scoreboard_GetTeamColor(i));
        html += "<div class='team-card color-" + colorStr + "' id='card" + String(i) + "'>";
        html += "<div class='team-name' id='lblName" + String(i) + "'>" + String(Scoreboard_GetTeamName(i)) + "</div>";
        html += "<div class='score' id='score" + String(i) + "'>" + String(Scoreboard_GetScore(i)) + "</div>";
        
        html += "<div class='btn-group'>";
        html += "<button class='btn-pos' onclick='addPts(" + String(i) + ", 50)'>+50</button>";
        html += "<button class='btn-pos' onclick='addPts(" + String(i) + ", 100)'>+100</button>";
        html += "<button class='btn-pos' onclick='addPts(" + String(i) + ", 150)'>+150</button>";
        html += "</div>";
        
        html += "<div class='btn-group'>";
        html += "<button class='btn-neg' onclick='addPts(" + String(i) + ", -50)'>-50</button>";
        html += "<button class='btn-neg' onclick='addPts(" + String(i) + ", -100)'>-100</button>";
        html += "<button class='btn-neg' onclick='addPts(" + String(i) + ", -150)'>-150</button>";
        html += "</div>";
        html += "</div>";
    }

    html += R"rawliteral(
        </div>
        <div class="log-box" id="activityLog">
            <div class="log-line">📋 Sistema Iniciado. Módulo Multi-Times Ativo.</div>
        </div>
    </div>

    <!-- TELA DE CONFIGURACAO -->
    <div id="config" class="content">
        <h2 style="margin-top:0;">Configurações</h2>
        <div class="config-list">
            
            <!-- Brilho do Painel -->
            <div class="config-item" style="border-left: 4px solid #ea580c; flex-direction:column; align-items:flex-start;">
                <div style="display:flex; justify-content:space-between; width:100%;">
                    <span>Brilho do Painel</span>
                    <span id="lblBright">)rawliteral" + String(bright * 100 / 2000) + R"rawliteral(%</span>
                </div>
                <input type="range" id="inpBright" min="10" max="2000" step="10" value=")rawliteral" + String(bright) + R"rawliteral(" onchange="updateBright(this.value)" oninput="document.getElementById('lblBright').innerText = Math.round(this.value * 100 / 2000) + '%'">
            </div>
            
            <div class="config-item" style="border-left: 4px solid #ea580c;">
                <span>Número de Equipes</span>
                <select id="inpNumTeams">
                    <option value="2" )rawliteral" + String(num == 2 ? "selected" : "") + R"rawliteral(>2 Times</option>
                    <option value="3" )rawliteral" + String(num == 3 ? "selected" : "") + R"rawliteral(>3 Times</option>
                    <option value="4" )rawliteral" + String(num == 4 ? "selected" : "") + R"rawliteral(>4 Times</option>
                </select>
            </div>
            <hr style="width:100%; border: 1px solid #333; margin: 5px 0;">
)rawliteral";

    for (int i = 0; i < num; i++) {
        String colorStr = getColorName(Scoreboard_GetTeamColor(i));
        html += "<div class='config-item'>";
        html += "<span style='width:100%; font-weight:bold; margin-bottom:5px;'>Equipe " + String(i+1) + "</span>";
        html += "<input type='text' id='inpName" + String(i) + "' maxlength='" + String(maxChars) + "' value='" + String(Scoreboard_GetTeamName(i)) + "'>";
        
        html += "<select id='inpColor" + String(i) + "'>";
        html += "<option value='red' " + String(colorStr == "red" ? "selected" : "") + ">Vermelho</option>";
        html += "<option value='green' " + String(colorStr == "green" ? "selected" : "") + ">Verde</option>";
        html += "<option value='blue' " + String(colorStr == "blue" ? "selected" : "") + ">Azul</option>";
        html += "<option value='yellow' " + String(colorStr == "yellow" ? "selected" : "") + ">Amarelo</option>";
        html += "<option value='cyan' " + String(colorStr == "cyan" ? "selected" : "") + ">Ciano</option>";
        html += "<option value='purple' " + String(colorStr == "purple" ? "selected" : "") + ">Roxo</option>";
        html += "<option value='white' " + String(colorStr == "white" ? "selected" : "") + ">Branco</option>";
        html += "</select>";
        html += "</div>";
    }

    html += R"rawliteral(
        </div>
        <button class="btn-save" onclick="saveConfig()">SALVAR CONFIGURAÇÕES</button>
        <button class="btn-reset" onclick="resetAll()">ZERAR PLACAR GERAL</button>
    </div>

    <!-- TELA DE DIAGNÓSTICO -->
    <div id="diag" class="content">
        <h2 style="margin-top:0;">Testes de Hardware</h2>
        <p style="text-align:center; max-width:400px; color:#aaa; font-size:0.95rem; margin-bottom:20px;">Use estes padrões visuais para identificar dormentes de cabos Flat, CIs com defeito, ou soldas frias no seu painel LED HUB75.</p>
        <div class="config-list">
            <button class="btn-save" style="margin-top:0; background:#3b82f6;" onclick="setTest(1)">1. Varredura Vertical (Colunas)</button>
            <button class="btn-save" style="margin-top:0; background:#3b82f6;" onclick="setTest(2)">2. Varredura Horizontal (Linhas)</button>
            <button class="btn-save" style="margin-top:0; background:#3b82f6;" onclick="setTest(3)">3. Teste Pixel a Pixel</button>
            <button class="btn-save" style="margin-top:0; background:#3b82f6;" onclick="setTest(4)">4. Cores Sólidas (Piscar Tela)</button>
            <button class="btn-reset" style="margin-top:20px; background:#ef4444;" onclick="setTest(0)">🛑 DESLIGAR TESTES (Voltar Placar)</button>
        </div>
    </div>

    <script>
        const NUM_TEAMS_SERVER = )rawliteral" + String(num) + R"rawliteral(;

        function switchTab(tabId, el) {
            document.querySelectorAll('.content').forEach(e => e.classList.remove('active'));
            document.querySelectorAll('.tab').forEach(e => e.classList.remove('active'));
            document.getElementById(tabId).classList.add('active');
            if(el) el.classList.add('active');
            else document.querySelector('.tabs').firstElementChild.classList.add('active');
        }

        function logActivity(msg) {
            let logBox = document.getElementById('activityLog');
            let now = new Date();
            let timeStr = now.getHours().toString().padStart(2,'0') + ':' + now.getMinutes().toString().padStart(2,'0') + ':' + now.getSeconds().toString().padStart(2,'0');
            let newLine = document.createElement('div');
            newLine.className = 'log-line';
            newLine.innerText = '[' + timeStr + '] ' + msg;
            logBox.insertBefore(newLine, logBox.firstChild);
        }

        function addPts(teamId, pts) {
            let scoreEl = document.getElementById('score'+teamId);
            let nameEl = document.getElementById('lblName'+teamId);
            let current = parseInt(scoreEl.innerText);
            let newScore = current + pts;
            if(newScore < 0) newScore = 0;
            if(newScore > 9950) newScore = 9950;
            
            scoreEl.innerText = newScore;
            let actionText = pts > 0 ? ("ganhou +" + pts) : ("perdeu " + Math.abs(pts));
            logActivity(nameEl.innerText + " " + actionText + " pontos. (Total: " + newScore + ")");
            
            fetch('/api/updateScore?team=' + teamId + '&score=' + newScore);
        }
        
        function updateBright(val) {
            fetch('/api/setBrightness?val=' + val);
        }

        function setTest(mode) {
            fetch('/api/setTestMode?mode=' + mode).then(() => {
                if(mode == 0) logActivity("Modo de Teste de Hardware DESLIGADO.");
                else logActivity("Modo de Teste " + mode + " ATIVADO no painel.");
            });
        }

        function saveConfig() {
            let n = parseInt(document.getElementById('inpNumTeams').value);
            
            if (n !== NUM_TEAMS_SERVER) {
                if(!confirm("⚠️ ATENÇÃO: Mudar o número de equipes vai ZERAR as pontuações atuais e REINICIAR o placar do zero. Tem certeza que deseja mudar?")) return;
                fetch('/api/setNumTeams?n=' + n).then(() => {
                    alert("Painel reiniciando! A página será atualizada em alguns segundos...");
                    setTimeout(() => location.reload(), 3000);
                });
                return; 
            }

            // Salvar nomes e cores
            for(let i=0; i<NUM_TEAMS_SERVER; i++) {
                let name = document.getElementById('inpName'+i).value.toUpperCase();
                let color = document.getElementById('inpColor'+i).value;
                
                document.getElementById('lblName'+i).innerText = name;
                document.getElementById('card'+i).className = 'team-card color-' + color;
                
                fetch('/api/updateName?team=' + i + '&name=' + name);
                fetch('/api/setColor?team=' + i + '&color=' + color);
            }
            logActivity("Configurações das equipes foram atualizadas e salvas.");
            alert("Nomes e Cores salvos na memória com sucesso!");
            switchTab('placar');
        }

        function resetAll() {
            if(confirm("ATENÇÃO: Zerar o placar não tem volta. Tem certeza?")) {
                for(let i=0; i<NUM_TEAMS_SERVER; i++) document.getElementById('score'+i).innerText = '0';
                logActivity("⚠️ PLACAR GERAL ZERADO PELO JUIZ ⚠️");
                fetch('/api/reset');
                switchTab('placar');
            }
        }
    </script>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
}

void handleUpdateScore() {
    if (server.hasArg("team") && server.hasArg("score")) {
        int team = server.arg("team").toInt();
        int score = server.arg("score").toInt();
        Scoreboard_SetScore(team, score);
        Scoreboard_Save();
        flagRedraw = true;
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void handleUpdateName() {
    if (server.hasArg("team") && server.hasArg("name")) {
        int team = server.arg("team").toInt();
        String name = server.arg("name");
        Scoreboard_SetTeamName(team, name.c_str());
        Scoreboard_Save();
        flagRedraw = true;
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void handleSetColor() {
    if (server.hasArg("team") && server.hasArg("color")) {
        int team = server.arg("team").toInt();
        int c = getColorValue(server.arg("color"));
        Scoreboard_SetTeamColor(team, c);
        Scoreboard_Save();
        flagRedraw = true;
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void handleSetBrightness() {
    if (server.hasArg("val")) {
        int val = server.arg("val").toInt();
        Scoreboard_SetBrightness(val);
        // Note: Brightness change takes effect immediately without needing full redraw
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void handleSetNumTeams() {
    if (server.hasArg("n")) {
        int n = server.arg("n").toInt();
        Scoreboard_SetNumTeams(n);
        server.send(200, "text/plain", "OK");
        delay(500);
        ESP.restart();
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void handleSetTestMode() {
    if (server.hasArg("mode")) {
        int mode = server.arg("mode").toInt();
        Scoreboard_SetTestMode(mode);
        flagRedraw = true; // Para forçar redesenho imediato do placar se modo for 0
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void handleReset() {
    Scoreboard_Clear();
    flagRedraw = true;
    server.send(200, "text/plain", "OK");
}

void handleMirror() {
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="pt-BR">
    <head>
        <meta charset="UTF-8">
        <title>ACAMP VOX - Live Mirror</title>
        <style>
            body { background: #000; color: #fff; text-align: center; font-family: sans-serif; margin: 0; padding: 20px; }
            canvas { width: 100%; max-width: 1024px; image-rendering: pixelated; border: 2px solid #ea580c; border-radius: 8px; margin-top: 20px; }
        </style>
    </head>
    <body>
        <h2>Live Mirror (Debug)</h2>
        <canvas id="screen" width="128" height="32"></canvas>
        <script>
            const canvas = document.getElementById('screen');
            const ctx = canvas.getContext('2d');
            const colors = ['#000000', '#ff0000', '#00ff00', '#ffff00', '#0000ff', '#ff00ff', '#00ffff', '#ffffff'];
            
            setInterval(() => {
                fetch('/api/screen')
                    .then(r => r.arrayBuffer())
                    .then(buf => {
                        const arr = new Uint8Array(buf);
                        ctx.clearRect(0, 0, 128, 32);
                        for(let x = 0; x < 128; x++) {
                            for(let y = 0; y < 32; y++) {
                                let colorIdx = arr[x * 32 + y] & 7;
                                if (colorIdx > 0) {
                                    ctx.fillStyle = colors[colorIdx];
                                    ctx.fillRect(x, y, 1, 1);
                                }
                            }
                        }
                    });
            }, 500);
        </script>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", html);
}

void handleScreen() {
    const uint8_t* buf = Display_GetBuffer();
    server.setContentLength(128 * 32);
    server.send(200, "application/octet-stream", "");
    WiFiClient client = server.client();
    client.write(buf, 128 * 32);
}

void WebServer_Init() {
    WiFi.softAP(ssid, password);
    
    server.on("/", handleRoot);
    server.on("/mirror", handleMirror);
    server.on("/api/screen", handleScreen);
    server.on("/api/updateScore", handleUpdateScore);
    server.on("/api/updateName", handleUpdateName);
    server.on("/api/setColor", handleSetColor);
    server.on("/api/setBrightness", handleSetBrightness);
    server.on("/api/setTestMode", handleSetTestMode);
    server.on("/api/setNumTeams", handleSetNumTeams);
    server.on("/api/reset", handleReset);
    server.begin();
}

int WebServer_GetCommand() {
    server.handleClient();
    return -1; 
}

bool WebServer_NeedsRedraw() {
    if(flagRedraw) {
        flagRedraw = false;
        return true;
    }
    return false;
}