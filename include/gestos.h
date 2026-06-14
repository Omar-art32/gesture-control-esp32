#pragma once
#include <Arduino.h>
#include <RevEng_PAJ7620.h>
#include "config.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Gestos — PAJ7620
//  Filtros anti-ruido:
//    1. Debounce global 350ms
//    2. Anti-repetición 1000ms
//    3. Ignorar rebote de eje opuesto en 450ms
// ═══════════════════════════════════════════════════════════════

namespace Gestos
{

    enum class Gesto : uint8_t
    {
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

    inline const char *nombreGesto(Gesto gesto)
    {
        switch (gesto)
        {
        case Gesto::IZQUIERDA:
            return "<- Izquierda";
        case Gesto::DERECHA:
            return "-> Derecha";
        case Gesto::ARRIBA:
            return "^ Arriba";
        case Gesto::ABAJO:
            return "v Abajo";
        case Gesto::ACERCAR:
            return "* Acercar";
        case Gesto::ALEJAR:
            return "* Alejar";
        case Gesto::HORARIO:
            return "@ Horario";
        case Gesto::ANTIHORARIO:
            return "@ Antihorario";
        case Gesto::WAVE:
            return "~ Wave";
        default:
            return "Esperando...";
        }
    }

    inline bool esHorizontal(Gesto g)
    {
        return g == Gesto::IZQUIERDA || g == Gesto::DERECHA;
    }

    inline bool esVertical(Gesto g)
    {
        return g == Gesto::ARRIBA || g == Gesto::ABAJO;
    }

    inline bool ejesOpuestos(Gesto a, Gesto b)
    {
        return (esHorizontal(a) && esVertical(b)) ||
               (esVertical(a) && esHorizontal(b));
    }

    // ── Estado interno ────────────────────────────────────────────
    static RevEng_PAJ7620 _sensor;
    static unsigned long _ultimoGestoMs = 0;
    static Gesto _ultimoAceptado = Gesto::NINGUNO;

    // ── Parámetros ────────────────────────────────────────────────
    constexpr unsigned long MS_DEBOUNCE = 350;
    constexpr unsigned long MS_REPETICION = 1000;
    constexpr unsigned long MS_IGNORAR_REBOTE = 450;

    inline bool iniciar()
    {
        // Pequeña espera para que el sensor estabilice el bus I2C
        delay(100);
        bool exito = _sensor.begin();
        if (!exito)
        {
            Serial.println("[PAJ7620] Sensor no encontrado.");
            return false;
        }
        Serial.println("[PAJ7620] Sensor inicializado.");
        return true;
    }

    inline Gesto _convertir(Gesture raw)
    {
        switch (raw)
        {
        case GES_LEFT:
            return Gesto::IZQUIERDA;
        case GES_RIGHT:
            return Gesto::DERECHA;
        case GES_UP:
            return Gesto::ARRIBA;
        case GES_DOWN:
            return Gesto::ABAJO;
        case GES_FORWARD:
            return Gesto::ACERCAR;
        case GES_BACKWARD:
            return Gesto::ALEJAR;
        case GES_CLOCKWISE:
            return Gesto::HORARIO;
        case GES_ANTICLOCKWISE:
            return Gesto::ANTIHORARIO;
        case GES_WAVE:
            return Gesto::WAVE;
        default:
            return Gesto::NINGUNO;
        }
    }

    inline Gesto leer()
    {
        Gesture raw = _sensor.readGesture();
        if (raw == GES_NONE)
            return Gesto::NINGUNO;

        Gesto detectado = _convertir(raw);
        if (detectado == Gesto::NINGUNO)
            return Gesto::NINGUNO;

        unsigned long ahora = millis();

        // ── Filtro 1: debounce global ─────────────────────────────
        if (ahora - _ultimoGestoMs < MS_DEBOUNCE)
        {
            return Gesto::NINGUNO;
        }

        // ── Filtro 2: anti-repetición ─────────────────────────────
        if (detectado == _ultimoAceptado &&
            ahora - _ultimoGestoMs < MS_REPETICION)
        {
            return Gesto::NINGUNO;
        }

        // ── Filtro 3: ignorar rebote de eje opuesto ───────────────
        if (_ultimoAceptado != Gesto::NINGUNO &&
            ejesOpuestos(_ultimoAceptado, detectado) &&
            ahora - _ultimoGestoMs < MS_IGNORAR_REBOTE)
        {
            Serial.printf("[Filtro] Rebote ignorado: %s\n",
                          nombreGesto(detectado));
            return Gesto::NINGUNO;
        }

        // ── Gesto aceptado ────────────────────────────────────────
        _ultimoGestoMs = ahora;
        _ultimoAceptado = detectado;

        Serial.printf("[Gesto] %s\n", nombreGesto(detectado));
        return detectado;
    }

} // namespace Gestos