#pragma once
#include <Arduino.h>
#include <BleKeyboard.h>
#include "led_rgb.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Perfiles — 4 modos de gestos
//  Wave cicla al siguiente modo
//  Basado en GestureController_Modos.pdf
// ═══════════════════════════════════════════════════════════════

namespace Perfiles
{

    // ── Modos disponibles ─────────────────────────────────────────
    enum class Modo : uint8_t
    {
        PRESENTACION = 0,
        SISTEMA = 1,
        MUSICA = 2,
        VENTANAS = 3,
        TOTAL = 4
    };

    // ── Índices de gesto (mismo orden que Gestos::Gesto) ─────────
    enum class Slot : uint8_t
    {
        IZQUIERDA = 0,
        DERECHA,
        ARRIBA,
        ABAJO,
        ACERCAR,
        ALEJAR,
        HORARIO,
        ANTIHORARIO,
        TOTAL
    };

    // ── Estructura de una acción ──────────────────────────────────
    struct Accion
    {
        const char *nombre; // texto para OLED y web
        uint8_t tecla;      // tecla principal
        uint8_t mod1;       // modificador 1 (0 = ninguno)
        uint8_t mod2;       // modificador 2 (0 = ninguno)
        uint8_t mod3;       // modificador 3 (0 = ninguno)
    };

    // ── Tabla de acciones [modo][slot] ────────────────────────────
    constexpr Accion TABLA[4][8] = {

        // ── MODO 0: PRESENTACION ──────────────────────────────────
        {
            {"Diapo Ant", KEY_LEFT_ARROW, 0, 0, 0},          // izq
            {"Diapo Sig", KEY_RIGHT_ARROW, 0, 0, 0},         // der
            {"Iniciar", KEY_F5, 0, 0, 0},                    // arr
            {"P. Negra", 'b', 0, 0, 0},                      // aba
            {"Reanudar", KEY_RETURN, 0, 0, 0},               // acer
            {"Terminar", KEY_ESC, 0, 0, 0},                  // alej
            {"Zoom +", KEY_UP_ARROW, KEY_LEFT_CTRL, 0, 0},   // hor
            {"Zoom -", KEY_DOWN_ARROW, KEY_LEFT_CTRL, 0, 0}, // antihor
        },

        // ── MODO 1: SISTEMA ───────────────────────────────────────
        {
            {"Vol -", KEY_F1, 0, 0, 0},                                     // izq
            {"Vol +", KEY_F2, 0, 0, 0},                                     // der
            {"Brillo +", KEY_F2, 0, 0, 0},                                  // arr
            {"Brillo -", KEY_F1, 0, 0, 0},                                  // aba
            {"Captura", KEY_LEFT_SHIFT, KEY_LEFT_GUI, 's', 0},              // acer
            {"Bloquear", 'l', KEY_LEFT_GUI, 0, 0},                          // alej
            {"Escrit ->", KEY_RIGHT_ARROW, KEY_LEFT_CTRL, KEY_LEFT_GUI, 0}, // hor
            {"Escrit <-", KEY_LEFT_ARROW, KEY_LEFT_CTRL, KEY_LEFT_GUI, 0},  // antihor
        },

        // ── MODO 2: MUSICA ────────────────────────────────────────
        {
            {"Ant Cancion", KEY_LEFT_ARROW, KEY_LEFT_CTRL, 0, 0},  // izq
            {"Sig Cancion", KEY_RIGHT_ARROW, KEY_LEFT_CTRL, 0, 0}, // der
            {"Vol +", KEY_F2, 0, 0, 0},                            // arr
            {"Vol -", KEY_F1, 0, 0, 0},                            // aba
            {"Play/Pause", 'p', KEY_LEFT_CTRL, 0, 0},              // acer
            {"Stop", 's', KEY_LEFT_CTRL, 0, 0},                    // alej
            {"Avanzar 10s", KEY_RIGHT_ARROW, 0, 0, 0},             // hor
            {"Retro 10s", KEY_LEFT_ARROW, 0, 0, 0},                // antihor
        },

        // ── MODO 3: VENTANAS ──────────────────────────────────────
        {
            {"Alt+Tab <-", KEY_TAB, KEY_LEFT_ALT, KEY_LEFT_SHIFT, 0}, // izq
            {"Alt+Tab ->", KEY_TAB, KEY_LEFT_ALT, 0, 0},              // der
            {"Maximizar", KEY_UP_ARROW, KEY_LEFT_GUI, 0, 0},          // arr
            {"Minimizar", KEY_DOWN_ARROW, KEY_LEFT_GUI, 0, 0},        // aba
            {"Cerrar", KEY_F4, KEY_LEFT_ALT, 0, 0},                   // acer
            {"Escritorio", 'd', KEY_LEFT_GUI, 0, 0},                  // alej
            {"Ventana ->", KEY_RIGHT_ARROW, KEY_LEFT_GUI, 0, 0},      // hor
            {"Ventana <-", KEY_LEFT_ARROW, KEY_LEFT_GUI, 0, 0},       // antihor
        },
    };

    // ── Nombres y colores de cada modo ────────────────────────────
    constexpr const char *NOMBRE_MODO[4] = {
        "PRESENTACION",
        "SISTEMA",
        "MUSICA",
        "VENTANAS"};

    constexpr LedRGB::Color COLOR_MODO[4] = {
        LedRGB::AZUL,     // PRESENTACION
        LedRGB::CYAN,     // SISTEMA
        LedRGB::MAGENTA,  // MUSICA
        LedRGB::AMARILLO, // VENTANAS
    };

    // ── Estado interno ────────────────────────────────────────────
    static uint8_t _modoActual = 0;

    // ── API pública ───────────────────────────────────────────────

    inline uint8_t modoActual()
    {
        return _modoActual;
    }

    inline const char *nombreModoActual()
    {
        return NOMBRE_MODO[_modoActual];
    }

    inline LedRGB::Color colorModoActual()
    {
        return COLOR_MODO[_modoActual];
    }

    // Cicla al siguiente modo (Wave)
    inline void siguienteModo()
    {
        _modoActual = (_modoActual + 1) % (uint8_t)Modo::TOTAL;
    }

    // Establece un modo específico (desde la web)
    inline void establecerModo(uint8_t modo)
    {
        if (modo < (uint8_t)Modo::TOTAL)
        {
            _modoActual = modo;
        }
    }

    // Obtiene la acción para el gesto y modo actuales
    inline const Accion &obtenerAccion(Slot slot)
    {
        return TABLA[_modoActual][(uint8_t)slot];
    }

} // namespace Perfiles