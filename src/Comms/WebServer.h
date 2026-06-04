#pragma once

void WebServer_Init();
int WebServer_GetCommand(); // Mantido para compatibilidade, mas retorna sempre -1 agora
bool WebServer_NeedsRedraw();