#pragma once
#include <Arduino.h>
#include <RevEng_PAJ7620.h>
#include "config.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Gestos
//  Librería: RevEng PAJ7620 @ ^1.4.1
//  Detecta los 9 gestos del sensor y los mapea a acciones BLE
// ═══════════════════════════════════════════════════════════════

namespace Gestos {

    // ── Tipos ────────────────────────────────────────────────────
    enum class Gesto : uint8_t {
        NINGUNO = 0,
        IZQUIERDA,
        DERECHA,
        ARRIBA,
        ABAJO,
        ACERCAR,
        ALEJAR,
        HORARIO,
        ANTIHORARIO,
        WAVE
    };

    // Nombre legible de cada gesto (para OLED y WebSocket)
    inline const char* nombreGesto(Gesto gesto) {
        switch (gesto) {
            case Gesto::IZQUIERDA:   return "<- Izquierda";
            case Gesto::DERECHA:     return "-> Derecha";
            case Gesto::ARRIBA:      return "^ Arriba";
            case Gesto::ABAJO:       return "v Abajo";
            case Gesto::ACERCAR:     return "* Acercar";
            case Gesto::ALEJAR:      return "* Alejar";
            case Gesto::HORARIO:     return "@ Horario";
            case Gesto::ANTIHORARIO: return "@ Antihorario";
            case Gesto::WAVE:        return "~ Wave";
            default:                 return "Esperando...";
        }
    }

    // ── Estado interno ────────────────────────────────────────────
    static RevEng_PAJ7620 _sensor;
    static unsigned long   _ultimoGesto = 0;

    // ── Inicialización ────────────────────────────────────────────
    inline bool iniciar() {
        bool exito = _sensor.begin();

        if (!exito) {
            Serial.println("[PAJ7620] Sensor no encontrado. Verifica el cableado I2C.");
            return false;
        }

        Serial.println("[PAJ7620] Sensor inicializado correctamente.");
        return true;
    }

    // ── Lectura con debounce ──────────────────────────────────────
    // Retorna Gesto::NINGUNO si no hay gesto nuevo o aún está en debounce
    inline Gesto leer() {
        Gesture lecturaRaw = _sensor.readGesture();

        if (lecturaRaw == GES_NONE) return Gesto::NINGUNO;

        unsigned long ahora = millis();
        if (ahora - _ultimoGesto < MS_DEBOUNCE) return Gesto::NINGUNO;

        _ultimoGesto = ahora;

        // Conversión de tipo de librería a nuestro enum
        switch (lecturaRaw) {
            case GES_LEFT:              return Gesto::IZQUIERDA;
            case GES_RIGHT:             return Gesto::DERECHA;
            case GES_UP:                return Gesto::ARRIBA;
            case GES_DOWN:              return Gesto::ABAJO;
            case GES_FORWARD:           return Gesto::ACERCAR;
            case GES_BACKWARD:          return Gesto::ALEJAR;
            case GES_CLOCKWISE:         return Gesto::HORARIO;
            case GES_ANTICLOCKWISE:     return Gesto::ANTIHORARIO;
            case GES_WAVE:              return Gesto::WAVE;
            default:                    return Gesto::NINGUNO;
        }
    }

} // namespace Gestos