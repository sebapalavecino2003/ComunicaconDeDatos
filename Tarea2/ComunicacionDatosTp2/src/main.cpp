#include <Arduino.h>
#include "IRProtocol.h"
#include "RGBControl.h"

// --- CONFIGURACIÓN DE HARDWARE ---
const int PIN_BUTTON = 4;
const int PIN_IR_SEND = 12; 
const int PIN_IR_RECV = 13;
const int PIN_ROLE_SELECT = 5; // HIGH: Maestro, LOW: Esclavo

RGBManager rgb(18, 19, 21);
bool initialSyncDone = false;

void setup() {
    Serial.begin(115200);
    rgb.begin();
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_ROLE_SELECT, INPUT_PULLUP);
    pinMode(PIN_IR_SEND, OUTPUT);
}

// Función para recibir y decodificar trama (Simulada para estructura)
bool receiveFrame(IRFrame &frame) {
    // Aquí iría tu lógica de lectura del TSOP4838
    return false; 
}

void loop() {
    bool isMaster = digitalRead(PIN_ROLE_SELECT);

    if (isMaster) {
        // --- LÓGICA MAESTRO ---
        
        // 1. Sincronización forzada al inicio
        if (!initialSyncDone) {
            Serial.println("Maestro: Solicitando estado al Esclavo para sincronizar...");
            IRFrame syncReq = {0x01, 0x03, 0, 0, 0, 0}; // 0x03 = SYNC_REQ
            syncReq.checksum = calculateChecksum(syncReq);
            sendFrame(syncReq);
            
            // Esperar brevemente la respuesta del esclavo
            IRFrame response;
            if (receiveFrame(response) && validateFrame(response)) {
                RGBColor slaveColor = {response.r, response.g, response.b};
                rgb.forceSync(slaveColor);
                initialSyncDone = true;
                Serial.println("Maestro: Sincronización inicial exitosa.");
            }
        }

        // 2. Control por pulsador
        if (digitalRead(PIN_BUTTON) == LOW) {
            delay(50); // Debounce
            RGBColor next = rgb.getNextInSequence();
            
            IRFrame frame = {0x01, 0x01, next.r, next.g, next.b, 0};
            frame.checksum = calculateChecksum(frame);
            
            Serial.println("Maestro: Enviando comando de cambio...");
            sendFrame(frame);

            // Solo actualiza localmente si el esclavo responde ACK (0x02)
            IRFrame ack;
            if (receiveFrame(ack) && ack.type == 0x02 && validateFrame(ack)) {
                rgb.setColor(next);
                Serial.println("Maestro: Cambio confirmado y guardado.");
            } else {
                Serial.println("Maestro Error: Esclavo no respondió. Reintentar.");
            }
            while(digitalRead(PIN_BUTTON) == LOW);
        }
    } else {
        // --- LÓGICA ESCLAVO ---
        IRFrame incoming;
        if (receiveFrame(incoming) && validateFrame(incoming)) {
            
            if (incoming.type == 0x01 || incoming.type == 0x03) {
                // Si es cambio (0x01) o petición de sync (0x03), mandamos nuestro color
                if (incoming.type == 0x01) {
                    RGBColor newColor = {incoming.r, incoming.g, incoming.b};
                    rgb.setColor(newColor);
                    Serial.println("Esclavo: Color actualizado.");
                }
                
                // Responder siempre con el estado actual (ACK/STATUS)
                RGBColor current = {incoming.r, incoming.g, incoming.b}; // O recuperar de memoria
                IRFrame reply = {0x02, 0x02, current.r, current.g, current.b, 0};
                reply.checksum = calculateChecksum(reply);
                sendFrame(reply);
            }
        }
    }
}