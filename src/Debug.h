#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG_MODE 1  // Altere para 0 para desligar todos os prints

#if DEBUG_MODE
  #define DEBUG_PRINT(x)   Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif

#endif
