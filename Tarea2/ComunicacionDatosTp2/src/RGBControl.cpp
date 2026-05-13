#include "RGBControl.h"
#include <Preferences.h>

// Objeto para manejar la memoria Flash del ESP32
Preferences prefs;

const RGBColor RGBManager::sequence[8] = {
    {255, 0, 0},     // Rojo
    {255, 255, 0},   // Amarillo
    {0, 255, 0},     // Verde
    {0, 255, 255},   // Celeste
    {0, 0, 255},     // Azul
    {255, 0, 255},   // Lila
    {255, 255, 255}, // Blanco
    {255, 192, 203}  // Rosa
};

RGBManager::RGBManager(int pR, int pG, int pB) : pinR(pR), pinG(pG), pinB(pB) {}

void RGBManager::begin() {
    pinMode(pinR, OUTPUT); 
    pinMode(pinG, OUTPUT); 
    pinMode(pinB, OUTPUT);

    // Abrir espacio "sistema" en modo lectura/escritura
    prefs.begin("sistema", false);

    // Recuperar último estado. Si no existe (primera vez), inicia en Rojo.
    RGBColor lastColor;
    lastColor.r = prefs.getUChar("r", 255);
    lastColor.g = prefs.getUChar("g", 0);
    lastColor.b = prefs.getUChar("b", 0);
    currentIndex = prefs.getInt("idx", 0);

    // Mostrar el color recuperado
    applyHardwareColor(lastColor);
}

void RGBManager::setColor(RGBColor color) {
    applyHardwareColor(color);
    
    // Guardar en memoria no volátil
    prefs.putUChar("r", color.r);
    prefs.putUChar("g", color.g);
    prefs.putUChar("b", color.b);
    prefs.putInt("idx", currentIndex);
}

void RGBManager::applyHardwareColor(RGBColor color) {
    analogWrite(pinR, color.r);
    analogWrite(pinG, color.g);
    analogWrite(pinB, color.b);
}

RGBColor RGBManager::getNextInSequence() {
    currentIndex = (currentIndex + 1) % 8;
    return sequence[currentIndex];
}

// Permite al Maestro forzar su índice al del Esclavo durante la sincronización
void RGBManager::forceSync(RGBColor target) {
    for(int i = 0; i < 8; i++) {
        if(sequence[i].r == target.r && sequence[i].g == target.g && sequence[i].b == target.b) {
            currentIndex = i;
            setColor(sequence[i]);
            break;
        }
    }
}