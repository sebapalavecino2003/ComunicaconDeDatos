#ifndef IR_PROTOCOL_H
#define IR_PROTOCOL_H

#include <Arduino.h>

// Tiempos de modulación en microsegundos (Pulse Distance Coding)
#define TIME_HEADER_MARK  2500
#define TIME_HEADER_SPACE 1000
#define TIME_BIT_MARK     500
#define TIME_ONE_SPACE    1500
#define TIME_ZERO_SPACE   500

// Estructura de trama empaquetada (48 bits totales)
struct __attribute__((packed)) IRFrame {
    uint8_t address;  // ID del dispositivo (4 bits efectivos)
    uint8_t type;     // 0x1: DATA, 0x2: ACK (4 bits efectivos)
    uint8_t r;        // Rojo (0-255)
    uint8_t g;        // Verde (0-255)
    uint8_t b;        // Azul (0-255)
    uint8_t checksum; // Verificación de integridad
};

uint8_t calculateChecksum(const IRFrame& frame);
bool validateFrame(const IRFrame& frame);

#endif