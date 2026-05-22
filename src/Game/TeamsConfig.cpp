#include "../../include/Config.h"

// ======================================================================
//  DEFINA OS SEUS TIMES AQUI
//  - name: até 3 letras maiúsculas (A-Z)
//  - color: use as constantes COR_* de Config.h
//
//  Para mudar o número de times, altere NUM_TEAMS em Config.h
//  e ajuste o array abaixo para ter exatamente NUM_TEAMS entradas.
// ======================================================================

const TeamConfig TIMES[NUM_TEAMS] = {
    { "TM1", COR_VERMELHO },   // Time 1 — vermelho
    { "TM2", COR_AZUL     },   // Time 2 — azul
    { "TM3", COR_AMARELO  },   // Time 3 — amarelo
    { "TM4", COR_VERDE    },   // Time 4 — verde
 // { "TM5", COR_CIANO    },   // Time 5 — descomente e mude NUM_TEAMS para 5
};

// ======================================================================
//  REFERÊNCIA DE CORES
//  COR_VERMELHO = 0   COR_AZUL    = 1   COR_VERDE   = 2
//  COR_AMARELO  = 3   COR_CIANO   = 4   COR_ROXO    = 5
//  COR_BRANCO   = 6
// ======================================================================
