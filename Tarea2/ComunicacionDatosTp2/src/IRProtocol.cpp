#include "IRProtocol.h"

// Pin de salida IR (definido externamente en main)
extern const int PIN_IR_SEND;

void transmit38kHz(uint32_t duration) {
    uint32_t start = micros();
    while (micros() - start < duration) {
        digitalWrite(PIN_IR_SEND, HIGH);
        delayMicroseconds(13); // Periodo para ~38kHz
        digitalWrite(PIN_IR_SEND, LOW);
        delayMicroseconds(13);
    }
}

void sendFrame(const IRFrame& frame) {
    // 1. Header
    transmit38kHz(TIME_HEADER_MARK);
    delayMicroseconds(TIME_HEADER_SPACE);

    // 2. Transmisión serializada
    uint8_t* bytes = (uint8_t*)&frame;
    for (size_t i = 0; i < sizeof(IRFrame); i++) {
        for (int bit = 7; bit >= 0; bit--) {
            transmit38kHz(TIME_BIT_MARK);
            if ((bytes[i] >> bit) & 0x01) {
                delayMicroseconds(TIME_ONE_SPACE);
            } else {
                delayMicroseconds(TIME_ZERO_SPACE);
            }
        }
    }
    // 3. Stop bit
    transmit38kHz(TIME_BIT_MARK);
}

uint8_t calculateChecksum(const IRFrame& frame) {
    return (uint8_t)((frame.address + frame.type + frame.r + frame.g + frame.b) & 0xFF);
}

bool validateFrame(const IRFrame& frame) {
    return calculateChecksum(frame) == frame.checksum;
}