#pragma once
#include <Arduino.h>
#include <BleKeyboard.h>
#include "led_rgb.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Perfiles — 4 modos + submodos de plataforma
//
//  Wave simple  → cicla entre modos
//  Wave doble   → dentro de PRESENTACION, cicla entre plataformas
//                 (PPT/LibreOffice → Google Slides → Canva → …)
//
//  El modo MUSICA usa teclas multimedia HID (MediaKeyReport)
//  en lugar de combinaciones de teclado, para que funcione
//  en cualquier reproductor (Spotify, VLC, navegador, etc.)
//  en cualquier OS sin depender de atajos de aplicación.
// ═══════════════════════════════════════════════════════════════

namespace Perfiles
{

    // ── Modos ─────────────────────────────────────────────────────
    enum class Modo : uint8_t
    {
        PRESENTACION = 0,
        SISTEMA      = 1,
        MUSICA       = 2,
        VENTANAS     = 3,
        TOTAL        = 4
    };

    // ── Submodos de plataforma (solo en PRESENTACION) ─────────────
    enum class Plataforma : uint8_t
    {
        PPT    = 0,   // PowerPoint / LibreOffice Impress
        GOOGLE = 1,   // Google Slides
        CANVA  = 2,   // Canva Presentations
        TOTAL  = 3
    };

    // ── Índices de gesto ──────────────────────────────────────────
    enum class Slot : uint8_t
    {
        IZQUIERDA   = 0,
        DERECHA,
        ARRIBA,
        ABAJO,
        ACERCAR,
        ALEJAR,
        HORARIO,
        ANTIHORARIO,
        TOTAL
    };

    // ── Tipos de acción ───────────────────────────────────────────
    enum class TipoAccion : uint8_t
    {
        TECLADO = 0,  // press(mod) + press(tecla) — combinación normal
        MEDIA   = 1,  // write(MediaKeyReport)     — tecla multimedia HID
        NINGUNA = 2   // no hace nada (slot sin asignar)
    };

    // ── Estructura de una acción ──────────────────────────────────
    struct Accion
    {
        const char  *nombre;
        TipoAccion   tipo;
        uint8_t      tecla;        // usado si tipo == TECLADO
        uint8_t      mod1;
        uint8_t      mod2;
        uint8_t      mod3;
        uint8_t      media[2];     // usado si tipo == MEDIA (= MediaKeyReport)
    };

    // ── Helper para construir acciones de teclado ─────────────────
    constexpr Accion accionTeclado(const char *n,
                                   uint8_t tecla,
                                   uint8_t m1 = 0,
                                   uint8_t m2 = 0,
                                   uint8_t m3 = 0)
    {
        return {n, TipoAccion::TECLADO, tecla, m1, m2, m3, {0, 0}};
    }

    // ── Helper para construir acciones multimedia ─────────────────
    constexpr Accion accionMedia(const char *n, uint8_t b0, uint8_t b1)
    {
        return {n, TipoAccion::MEDIA, 0, 0, 0, 0, {b0, b1}};
    }

    // ── Tablas de presentación por plataforma ─────────────────────
    //
    //  Acción            │ PPT / LibreOffice  │ Google Slides    │ Canva
    //  ──────────────────┼────────────────────┼──────────────────┼─────────────────
    //  Diapo anterior    │ ←                  │ ←                │ ←
    //  Diapo siguiente   │ →                  │ →                │ →
    //  Iniciar           │ F5                 │ Ctrl+Shift+F5    │ Ctrl+Alt+P
    //  Pantalla negra    │ B                  │ B                │ — (no existe;
    //                    │                    │                  │   usamos Alt+P
    //                    │                    │                  │   = presenter view)
    //  Reanudar          │ Enter              │ Enter            │ Enter
    //  Terminar          │ Esc                │ Esc              │ Esc
    //  Zoom + (editor)   │ Ctrl+=             │ Ctrl+=           │ Ctrl+=
    //  Zoom - (editor)   │ Ctrl+-             │ Ctrl+-           │ Ctrl+-

    constexpr Accion TABLA_PPT[8] = {
        accionTeclado("Diapo Ant", KEY_LEFT_ARROW),
        accionTeclado("Diapo Sig", KEY_RIGHT_ARROW),
        accionTeclado("Iniciar",   KEY_F5),
        accionTeclado("P. Negra",  'b'),
        accionTeclado("Reanudar",  KEY_RETURN),
        accionTeclado("Terminar",  KEY_ESC),
        accionTeclado("Zoom +",    '=', KEY_LEFT_CTRL),
        accionTeclado("Zoom -",    '-', KEY_LEFT_CTRL),
    };

    constexpr Accion TABLA_GOOGLE[8] = {
        accionTeclado("Diapo Ant", KEY_LEFT_ARROW),
        accionTeclado("Diapo Sig", KEY_RIGHT_ARROW),
        accionTeclado("Iniciar",   KEY_F5,    KEY_LEFT_CTRL, KEY_LEFT_SHIFT),
        accionTeclado("P. Negra",  'b'),
        accionTeclado("Reanudar",  KEY_RETURN),
        accionTeclado("Terminar",  KEY_ESC),
        accionTeclado("Zoom +",    '=', KEY_LEFT_CTRL),
        accionTeclado("Zoom -",    '-', KEY_LEFT_CTRL),
    };

    constexpr Accion TABLA_CANVA[8] = {
        accionTeclado("Diapo Ant", KEY_LEFT_ARROW),
        accionTeclado("Diapo Sig", KEY_RIGHT_ARROW),
        accionTeclado("Iniciar",   'p',        KEY_LEFT_CTRL, KEY_LEFT_ALT),
        accionTeclado("Ver Notas", 'p',        KEY_LEFT_ALT),   // presenter view
        accionTeclado("Reanudar",  KEY_RETURN),
        accionTeclado("Terminar",  KEY_ESC),
        accionTeclado("Zoom +",    '=', KEY_LEFT_CTRL),
        accionTeclado("Zoom -",    '-', KEY_LEFT_CTRL),
    };

    // ── Tabla general [modo][slot] ────────────────────────────────
    constexpr Accion TABLA[4][8] = {

        // MODO 0: PRESENTACION — delegado a TABLA_PPT/GOOGLE/CANVA en obtenerAccion()
        {
            accionTeclado("Diapo Ant", KEY_LEFT_ARROW),
            accionTeclado("Diapo Sig", KEY_RIGHT_ARROW),
            accionTeclado("Iniciar",   KEY_F5),
            accionTeclado("P. Negra",  'b'),
            accionTeclado("Reanudar",  KEY_RETURN),
            accionTeclado("Terminar",  KEY_ESC),
            accionTeclado("Zoom +",    '=', KEY_LEFT_CTRL),
            accionTeclado("Zoom -",    '-', KEY_LEFT_CTRL),
        },

        // MODO 1: SISTEMA
        {
            accionTeclado("Vol -",     KEY_F1),
            accionTeclado("Vol +",     KEY_F2),
            accionTeclado("Brillo +",  KEY_F6),
            accionTeclado("Brillo -",  KEY_F5),
            accionTeclado("Captura",   KEY_LEFT_SHIFT, KEY_LEFT_GUI, 's'),
            accionTeclado("Bloquear",  'l',            KEY_LEFT_GUI),
            accionTeclado("Escrit ->", KEY_RIGHT_ARROW, KEY_LEFT_CTRL, KEY_LEFT_GUI),
            accionTeclado("Escrit <-", KEY_LEFT_ARROW,  KEY_LEFT_CTRL, KEY_LEFT_GUI),
        },

        // MODO 2: MUSICA — teclas multimedia HID
        // Funcionan en Spotify, VLC, navegador con YouTube, Apple Music, etc.
        // en Windows, macOS y Linux sin depender de atajos de aplicación.
        //
        //  KEY_MEDIA_PREVIOUS_TRACK = {2, 0}
        //  KEY_MEDIA_NEXT_TRACK     = {1, 0}
        //  KEY_MEDIA_VOLUME_UP      = {32, 0}
        //  KEY_MEDIA_VOLUME_DOWN    = {64, 0}
        //  KEY_MEDIA_PLAY_PAUSE     = {8, 0}
        //  KEY_MEDIA_STOP           = {4, 0}
        // Para seek ±10s no hay tecla HID estándar — se usa → / ← que
        // funciona en Spotify (5s) y en la mayoría de reproductores.
        {
            accionMedia("Ant Cancion", 2,  0),   // KEY_MEDIA_PREVIOUS_TRACK
            accionMedia("Sig Cancion", 1,  0),   // KEY_MEDIA_NEXT_TRACK
            accionMedia("Vol +",       32, 0),   // KEY_MEDIA_VOLUME_UP
            accionMedia("Vol -",       64, 0),   // KEY_MEDIA_VOLUME_DOWN
            accionMedia("Play/Pause",  8,  0),   // KEY_MEDIA_PLAY_PAUSE
            accionMedia("Stop",        4,  0),   // KEY_MEDIA_STOP
            accionTeclado("Avanzar",   KEY_RIGHT_ARROW),  // seek → (no hay HID estándar)
            accionTeclado("Retroceder",KEY_LEFT_ARROW),   // seek ←
        },

        // MODO 3: VENTANAS
        {
            accionTeclado("Alt+Tab <-",  KEY_TAB,         KEY_LEFT_ALT, KEY_LEFT_SHIFT),
            accionTeclado("Alt+Tab ->",  KEY_TAB,         KEY_LEFT_ALT),
            accionTeclado("Maximizar",   KEY_UP_ARROW,    KEY_LEFT_GUI),
            accionTeclado("Minimizar",   KEY_DOWN_ARROW,  KEY_LEFT_GUI),
            accionTeclado("Cerrar",      KEY_F4,          KEY_LEFT_ALT),
            accionTeclado("Escritorio",  'd',             KEY_LEFT_GUI),
            accionTeclado("Ventana ->",  KEY_RIGHT_ARROW, KEY_LEFT_GUI),
            accionTeclado("Ventana <-",  KEY_LEFT_ARROW,  KEY_LEFT_GUI),
        },
    };

    // ── Nombres, colores ──────────────────────────────────────────
    constexpr const char *NOMBRE_MODO[4] = {
        "PRESENTACION",
        "SISTEMA",
        "MUSICA",
        "VENTANAS"
    };

    constexpr const char *NOMBRE_PLATAFORMA[3] = {
        "PPT / LO",
        "GOOGLE",
        "CANVA"
    };

    constexpr LedRGB::Color COLOR_MODO[4] = {
        LedRGB::AZUL,
        LedRGB::CYAN,
        LedRGB::MAGENTA,
        LedRGB::AMARILLO,
    };

    // ── Estado interno ────────────────────────────────────────────
    static uint8_t _modoActual       = 0;
    static uint8_t _plataformaActual = 0;

    // ── API pública ───────────────────────────────────────────────
    inline uint8_t modoActual()       { return _modoActual; }
    inline uint8_t plataformaActual() { return _plataformaActual; }

    inline bool esModoPresentation()
    {
        return _modoActual == (uint8_t)Modo::PRESENTACION;
    }

    inline const char *nombreModoActual()
    {
        if (esModoPresentation())
            return NOMBRE_PLATAFORMA[_plataformaActual];
        return NOMBRE_MODO[_modoActual];
    }

    inline LedRGB::Color colorModoActual()
    {
        return COLOR_MODO[_modoActual];
    }

    inline void siguienteModo()
    {
        _modoActual = (_modoActual + 1) % (uint8_t)Modo::TOTAL;
    }

    inline bool siguientePlataforma()
    {
        if (!esModoPresentation()) return false;
        _plataformaActual = (_plataformaActual + 1) % (uint8_t)Plataforma::TOTAL;
        return true;
    }

    inline void establecerModo(uint8_t modo)
    {
        if (modo < (uint8_t)Modo::TOTAL)
            _modoActual = modo;
    }

    inline void establecerPlataforma(uint8_t plataforma)
    {
        if (plataforma < (uint8_t)Plataforma::TOTAL)
            _plataformaActual = plataforma;
    }

    inline const Accion &obtenerAccion(Slot slot)
    {
        if (esModoPresentation())
        {
            switch ((Plataforma)_plataformaActual)
            {
            case Plataforma::PPT:    return TABLA_PPT[(uint8_t)slot];
            case Plataforma::GOOGLE: return TABLA_GOOGLE[(uint8_t)slot];
            case Plataforma::CANVA:  return TABLA_CANVA[(uint8_t)slot];
            default:                 return TABLA_PPT[(uint8_t)slot];
            }
        }
        return TABLA[_modoActual][(uint8_t)slot];
    }

} // namespace Perfiles