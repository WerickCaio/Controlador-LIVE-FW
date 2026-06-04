#include "WebServer.h"
#include <WiFi.h>
#include <WebServer.h>
#include "../Game/Scoreboard.h"

WebServer server(80);

const char* ssid = "ACAMP_VOX";
const char* password = "";
bool flagRedraw = false;

void handleRoot() {
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
        .t0 { border-top: 6px solid #ef4444; } .t1 { border-top: 6px solid #3b82f6; }
        .t2 { border-top: 6px solid #eab308; } .t3 { border-top: 6px solid #22c55e; }
        .team-name { font-size: 1.3rem; font-weight: bold; margin-bottom: 5px; text-transform: uppercase; }
        .score { font-size: 2.5rem; font-weight: bold; margin: 10px 0; }
        
        .btn-group { display: flex; gap: 5px; margin-bottom: 5px; }
        .btn-group button { flex: 1; padding: 10px 2px; border: none; border-radius: 4px; color: white; font-weight: bold; cursor: pointer; font-size: 0.95rem; }
        
        .t0 .btn-pos { background: #dc2626; } .t0 .btn-neg { background: #991b1b; }
        .t1 .btn-pos { background: #2563eb; } .t1 .btn-neg { background: #1e3a8a; }
        .t2 .btn-pos { background: #ca8a04; } .t2 .btn-neg { background: #854d0e; }
        .t3 .btn-pos { background: #16a34a; } .t3 .btn-neg { background: #14532d; }
        
        .log-box { margin-top: 30px; width: 100%; max-width: 600px; background: #1e1e24; border-radius: 8px; padding: 10px; height: 180px; overflow-y: auto; font-family: monospace; font-size: 0.95rem; color: #aaa; border-left: 4px solid #ea580c; box-sizing: border-box;}
        .log-line { border-bottom: 1px solid #333; padding: 8px 0; }
        
        .config-list { display: flex; flex-direction: column; gap: 15px; width: 100%; max-width: 400px; }
        .config-item { display: flex; align-items: center; justify-content: space-between; background: #1e1e24; padding: 15px; border-radius: 8px; }
        .config-item input { width: 80px; padding: 8px; font-weight: bold; text-align: center; text-transform: uppercase; font-size: 1.1rem; border: none; border-radius: 4px; background:#121212; color:white; }
        .btn-save { background: #ea580c; border: none; color: white; padding: 15px 20px; font-size: 1.1rem; font-weight: bold; border-radius: 6px; cursor: pointer; margin-top: 20px; width: 100%; max-width: 400px;}
        .btn-reset { background: #991b1b; border: none; color: white; padding: 15px 20px; font-size: 1.1rem; font-weight: bold; border-radius: 6px; cursor: pointer; margin-top: 40px; width: 100%; max-width: 400px;}
    </style>
</head>
<body>
    <div class="tabs">
        <div class="tab active" onclick="switchTab('placar', this)">🎮 Placar</div>
        <div class="tab" onclick="switchTab('config', this)">⚙️ Configurações</div>
    </div>

    <!-- TELA DO PLACAR -->
    <div id="placar" class="content active">
        <div class="dashboard">
)rawliteral";

    const char* colors[4] = {"t0", "t1", "t2", "t3"};
    for (int i = 0; i < 4; i++) {
        html += "<div class='team-card " + String(colors[i]) + "'>";
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
            <div class="log-line">📋 Sistema Iniciado. Pronto para rastreio.</div>
        </div>
    </div>

    <!-- TELA DE CONFIGURACAO -->
    <div id="config" class="content">
        <h2 style="margin-top:0;">Nomes das Equipes</h2>
        <div class="config-list">
)rawliteral";

    for (int i = 0; i < 4; i++) {
        html += "<div class='config-item " + String(colors[i]) + "'>";
        html += "<span>Equipe " + String(i+1) + "</span>";
        html += "<input type='text' id='inpName" + String(i) + "' maxlength='3' value='" + String(Scoreboard_GetTeamName(i)) + "'>";
        html += "</div>";
    }

    html += R"rawliteral(
        </div>
        <button class="btn-save" onclick="saveNames()">SALVAR NOMES NA MEMÓRIA</button>
        <button class="btn-reset" onclick="resetAll()">ZERAR PLACAR GERAL</button>
    </div>

    <script>
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

        function saveNames() {
            for(let i=0; i<4; i++) {
                let name = document.getElementById('inpName'+i).value.toUpperCase();
                document.getElementById('lblName'+i).innerText = name;
                fetch('/api/updateName?team=' + i + '&name=' + name);
            }
            logActivity("Nomes das equipes foram atualizados e salvos.");
            alert("Nomes salvos na memória com sucesso!");
            switchTab('placar');
        }

        function resetAll() {
            if(confirm("ATENÇÃO: Zerar o placar não tem volta. Tem certeza?")) {
                for(let i=0; i<4; i++) document.getElementById('score'+i).innerText = '0';
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
        Scoreboard_Save(); // Grava na NVS as novas definicoes de strings
        flagRedraw = true;
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

void WebServer_Init() {
    WiFi.softAP(ssid, password);
    
    server.on("/", handleRoot);
    server.on("/api/updateScore", handleUpdateScore);
    server.on("/api/updateName", handleUpdateName);
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