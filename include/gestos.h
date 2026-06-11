#pragma once
#include <Arduino.h>
#include <RevEng_PAJ7620.h>
#include "config.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Gestos — PAJ7620
//
//  Filtros anti-ruido:
//    1. Debounce global 350ms
//    2. Anti-repetición 1000ms  (WAVE tiene su propio valor: 400ms)
//    3. Ignorar rebote de eje opuesto en 450ms
//
//  Wave simple  → waveCount == 1
//  Wave doble   → waveCount >= 2
//    El conteo viene del hardware (registro 0x43 del PAJ7620),
//    se lee en el mismo ciclo que readGesture() antes de que el
//    sensor limpie el registro. No requiere detectar dos gestos
//    separados, por lo que no lucha contra los filtros de debounce.
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
        WAVE        // consultar ultimoWaveCount() para saber si fue doble
    };

    inline const char *nombreGesto(Gesto gesto)
    {
        switch (gesto)
        {
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
               (esVertical(a)   && esHorizontal(b));
    }

    // ── Estado interno ────────────────────────────────────────────
    static RevEng_PAJ7620 _sensor;
    static unsigned long  _ultimoGestoMs   = 0;
    static Gesto          _ultimoAceptado  = Gesto::NINGUNO;
    static int            _ultimoWaveCnt   = 0; // conteo del último wave aceptado

    // ── Parámetros ────────────────────────────────────────────────
    constexpr unsigned long MS_DEBOUNCE       = 350;
    constexpr unsigned long MS_REPETICION     = 1000;
    constexpr unsigned long MS_REPETICION_WAVE = 400; // wave puede repetirse más rápido
    constexpr unsigned long MS_IGNORAR_REBOTE = 450;

    // ── Umbral de wave doble ──────────────────────────────────────
    // El PAJ7620 reporta cuántas veces se agitó la mano (0..15).
    // Un movimiento relajado suele dar 1-2; agitar bien da 3+.
    // Usamos >= 2 como umbral para "wave doble".
    constexpr int WAVE_DOBLE_UMBRAL = 2;

    inline bool iniciar()
    {
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

    // Devuelve el waveCount del último WAVE aceptado.
    // Llamar sólo cuando leer() haya retornado Gesto::WAVE.
    inline int ultimoWaveCount()
    {
        return _ultimoWaveCnt;
    }

    // true si el último wave aceptado fue "doble" (count >= umbral)
    inline bool ultimoWaveDoble()
    {
        return _ultimoWaveCnt >= WAVE_DOBLE_UMBRAL;
    }

    inline Gesto _convertir(Gesture raw)
    {
        switch (raw)
        {
        case GES_LEFT:          return Gesto::IZQUIERDA;
        case GES_RIGHT:         return Gesto::DERECHA;
        case GES_UP:            return Gesto::ARRIBA;
        case GES_DOWN:          return Gesto::ABAJO;
        case GES_FORWARD:       return Gesto::ACERCAR;
        case GES_BACKWARD:      return Gesto::ALEJAR;
        case GES_CLOCKWISE:     return Gesto::HORARIO;
        case GES_ANTICLOCKWISE: return Gesto::ANTIHORARIO;
        case GES_WAVE:          return Gesto::WAVE;
        default:                return Gesto::NINGUNO;
        }
    }

    inline Gesto leer()
    {
        Gesture raw = _sensor.readGesture();
        if (raw == GES_NONE)
            return Gesto::NINGUNO;

        // Leer waveCount ANTES de cualquier filtro — el registro
        // del sensor puede limpiarse en el siguiente ciclo I2C.
        // Para gestos no-wave el valor será 0 y se ignora.
        int waveCount = 0;
        if (raw == GES_WAVE)
            waveCount = _sensor.getWaveCount();

        Gesto detectado = _convertir(raw);
        if (detectado == Gesto::NINGUNO)
            return Gesto::NINGUNO;

        unsigned long ahora = millis();

        // ── Filtro 1: debounce global ─────────────────────────────
        if (ahora - _ultimoGestoMs < MS_DEBOUNCE)
            return Gesto::NINGUNO;

        // ── Filtro 2: anti-repetición ─────────────────────────────
        // Wave usa una ventana más corta para no bloquear el doble wave
        // en caso de que el usuario haga dos waves rápidos consecutivos
        // que el sensor reporte como dos eventos GES_WAVE separados.
        unsigned long msRep = (detectado == Gesto::WAVE)
                              ? MS_REPETICION_WAVE
                              : MS_REPETICION;

        if (detectado == _ultimoAceptado &&
            ahora - _ultimoGestoMs < msRep)
            return Gesto::NINGUNO;

        // ── Filtro 3: ignorar rebote de eje opuesto ───────────────
        if (_ultimoAceptado != Gesto::NINGUNO &&
            ejesOpuestos(_ultimoAceptado, detectado) &&
            ahora - _ultimoGestoMs < MS_IGNORAR_REBOTE)
        {
            Serial.printf("[Filtro] Rebote ignorado: %s\n", nombreGesto(detectado));
            return Gesto::NINGUNO;
        }

        // ── Gesto aceptado ────────────────────────────────────────
        _ultimoGestoMs  = ahora;
        _ultimoAceptado = detectado;
        _ultimoWaveCnt  = waveCount;  // 0 para no-wave

        if (detectado == Gesto::WAVE)
            Serial.printf("[Gesto] Wave (count=%d, doble=%s)\n",
                          waveCount, waveCount >= WAVE_DOBLE_UMBRAL ? "SI" : "no");
        else
            Serial.printf("[Gesto] %s\n", nombreGesto(detectado));

        return detectado;
    }

} // namespace Gestos