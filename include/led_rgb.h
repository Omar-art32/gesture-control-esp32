#pragma once
#include <Arduino.h>
#include "config.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo LED RGB
//  Usa ledcWrite() nativo del ESP32 — sin librería externa
//  Cátodo común: valor 0 = apagado, 255 = máximo brillo
// ═══════════════════════════════════════════════════════════════

namespace LedRGB
{

    struct Color
    {
        uint8_t rojo;
        uint8_t verde;
        uint8_t azul;
    };

    constexpr Color APAGADO = {0, 0, 0};
    constexpr Color BLANCO = {255, 255, 255};
    constexpr Color ROJO = {255, 0, 0};
    constexpr Color VERDE = {0, 255, 0};
    constexpr Color AZUL = {0, 0, 255};
    constexpr Color AMARILLO = {255, 200, 0};
    constexpr Color CYAN = {0, 200, 255};
    constexpr Color MAGENTA = {255, 0, 180};
    constexpr Color NARANJA = {255, 80, 0};

    constexpr Color COLOR_CONECTADO = VERDE;
    constexpr Color COLOR_ESPERANDO = AZUL;
    constexpr Color COLOR_GESTO = AMARILLO;
    constexpr Color COLOR_ERROR = ROJO;

    // ── establecerColor va ANTES de iniciar() para que pueda llamarla ──
    inline void establecerColor(Color color)
    {
        ledcWrite(CANAL_ROJO, color.rojo);
        ledcWrite(CANAL_VERDE, color.verde);
        ledcWrite(CANAL_AZUL, color.azul);
    }

    inline void iniciar()
    {
        ledcSetup(CANAL_ROJO, PWM_FRECUENCIA, PWM_RESOLUCION);
        ledcSetup(CANAL_VERDE, PWM_FRECUENCIA, PWM_RESOLUCION);
        ledcSetup(CANAL_AZUL, PWM_FRECUENCIA, PWM_RESOLUCION);

        ledcAttachPin(PIN_ROJO, CANAL_ROJO);
        ledcAttachPin(PIN_VERDE, CANAL_VERDE);
        ledcAttachPin(PIN_AZUL, CANAL_AZUL);

        establecerColor(APAGADO);
    }

    inline void parpadear(Color color, uint8_t veces = 2, uint16_t ms = 150)
    {
        for (uint8_t i = 0; i < veces; i++)
        {
            establecerColor(color);
            delay(ms);
            establecerColor(APAGADO);
            delay(ms);
        }
    }

} // namespace LedRGB