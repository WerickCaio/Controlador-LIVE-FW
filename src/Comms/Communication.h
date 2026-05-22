#pragma once

// Inicializa a comunicação serial (USB e, se habilitado, Bluetooth).
void Comm_Init();

// Retorna o próximo comando recebido, ou -1 se nenhum.
int  Comm_GetCommand();
