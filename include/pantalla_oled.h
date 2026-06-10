#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Pantalla OLED — Robot bailando + pizarra de gestos
//  OLED 128x64 dividida en dos zonas:
//    Izquierda (0–30px)  → robot animado 4 frames
//    Derecha  (34–128px) → pizarra con nombre e ícono del gesto
//  100% no bloqueante — cero delay()
// ═══════════════════════════════════════════════════════════════

namespace PantallaOLED
{

    // ── Velocidad de animación ────────────────────────────────────
    constexpr unsigned long MS_FRAME_LENTO = 250; // 4fps  — esperando
    constexpr unsigned long MS_FRAME_RAPIDO = 66; // 15fps — gesto activo

    // ── Estado interno ────────────────────────────────────────────
    static Adafruit_SSD1306 _pantalla(OLED_ANCHO, OLED_ALTO, &Wire, -1);
    static bool _disponible = false;
    static uint8_t _frame = 0; // 0–3
    static unsigned long _tiempoFrame = 0;
    static unsigned long _tiempoGesto = 0;
    static bool _gestoActivo = false;
    static char _nombreGesto[16] = "Esperando...";
    static uint8_t _iconoGesto = 0; // ver enum abajo

    constexpr unsigned long MS_GESTO_VISIBLE = 1200; // cuánto queda el gesto en pizarra

    // ── Íconos de tecla ───────────────────────────────────────────
    enum class Icono : uint8_t
    {
        NINGUNO = 0,
        SIGUIENTE,  // ▶|
        ANTERIOR,   // |◀
        VOL_MAS,    // volumen +
        VOL_MENOS,  // volumen -
        PLAY_PAUSE, // ▶/‖
        FULLSCREEN, // F11
        ESCAPE,     // ESC
        SCROLL_ABA, // ↓↓↓
        SCROLL_ARR, // ↑↑↑
    };

    static Icono _icono = Icono::NINGUNO;

    // ── Inicialización ────────────────────────────────────────────
    inline bool iniciar()
    {
        Wire.begin(OLED_SDA, OLED_SCL);
        _disponible = _pantalla.begin(SSD1306_SWITCHCAPVCC, OLED_DIRECCION);
        if (!_disponible)
        {
            Serial.println("[OLED] No encontrada en 0x3C");
            return false;
        }
        _pantalla.clearDisplay();
        _pantalla.display();
        return true;
    }

    // ════════════════════════════════════════════════════════════
    //  ROBOT — 4 frames de baile
    //  Zona izquierda: x=0..30, y=0..63
    //  Centro del robot: x=15
    // ════════════════════════════════════════════════════════════

    // Dibuja la cabeza del robot (siempre igual)
    inline void _cabeza(int8_t ox)
    {
        // antena
        _pantalla.drawFastVLine(15 + ox, 4, 5, SSD1306_WHITE);
        _pantalla.fillCircle(15 + ox, 3, 2, SSD1306_WHITE);
        // cabeza
        _pantalla.fillRoundRect(7 + ox, 9, 16, 13, 2, SSD1306_WHITE);
        // ojos
        _pantalla.fillRect(9 + ox, 11, 4, 5, SSD1306_BLACK);
        _pantalla.fillRect(17 + ox, 11, 4, 5, SSD1306_BLACK);
        // boca
        _pantalla.fillRect(11 + ox, 18, 6, 2, SSD1306_BLACK);
    }

    // Dibuja el cuerpo (siempre igual)
    inline void _cuerpo(int8_t ox)
    {
        _pantalla.fillRoundRect(8 + ox, 22, 14, 13, 2, SSD1306_WHITE);
    }

    // Dibuja las piernas según frame
    inline void _piernas(uint8_t frame)
    {
        // frame 0,2 → piernas juntas
        // frame 1   → pierna izq adelante
        // frame 3   → pierna der adelante
        switch (frame)
        {
        case 0:
        case 2:
            _pantalla.fillRoundRect(9, 35, 5, 8, 1, SSD1306_WHITE);
            _pantalla.fillRoundRect(16, 35, 5, 8, 1, SSD1306_WHITE);
            break;
        case 1:
            _pantalla.fillRoundRect(7, 35, 5, 8, 1, SSD1306_WHITE);
            _pantalla.fillRoundRect(17, 35, 5, 8, 1, SSD1306_WHITE);
            break;
        case 3:
            _pantalla.fillRoundRect(11, 35, 5, 8, 1, SSD1306_WHITE);
            _pantalla.fillRoundRect(14, 35, 5, 8, 1, SSD1306_WHITE);
            break;
        }
    }

    // Dibuja los brazos según frame
    // frame 0 → brazos abajo
    // frame 1 → brazo der arriba
    // frame 2 → ambos arriba
    // frame 3 → brazo izq arriba
    inline void _brazos(uint8_t frame)
    {
        switch (frame)
        {
        case 0:
            _pantalla.fillRoundRect(3, 23, 4, 9, 2, SSD1306_WHITE);
            _pantalla.fillRoundRect(23, 23, 4, 9, 2, SSD1306_WHITE);
            break;
        case 1:
            _pantalla.fillRoundRect(3, 23, 4, 9, 2, SSD1306_WHITE);
            _pantalla.fillRoundRect(23, 15, 4, 9, 2, SSD1306_WHITE);
            break;
        case 2:
            _pantalla.fillRoundRect(3, 15, 4, 9, 2, SSD1306_WHITE);
            _pantalla.fillRoundRect(23, 15, 4, 9, 2, SSD1306_WHITE);
            break;
        case 3:
            _pantalla.fillRoundRect(3, 15, 4, 9, 2, SSD1306_WHITE);
            _pantalla.fillRoundRect(23, 23, 4, 9, 2, SSD1306_WHITE);
            break;
        }
    }

    // Dibuja el robot completo en el frame actual
    // ox = offset horizontal para pequeño balanceo
    inline void _dibujarRobot(uint8_t frame)
    {
        // balanceo: frames 1 y 3 se desplazan ±1px
        int8_t ox = (frame == 1) ? 1 : (frame == 3) ? -1
                                                    : 0;
        _brazos(frame);
        _cuerpo(ox);
        _cabeza(ox);
        _piernas(frame);
    }

    // ════════════════════════════════════════════════════════════
    //  PIZARRA — zona derecha x=34..127
    // ════════════════════════════════════════════════════════════

    // Dibuja el ícono de la tecla enviada
    inline void _dibujarIcono(Icono icono, int16_t x, int16_t y)
    {
        switch (icono)
        {

        case Icono::SIGUIENTE:
            // ▶|
            _pantalla.fillTriangle(x, y + 4, x + 6, y + 8, x, y + 12, SSD1306_WHITE);
            _pantalla.fillRect(x + 7, y + 4, 2, 9, SSD1306_WHITE);
            break;

        case Icono::ANTERIOR:
            // |◀
            _pantalla.fillRect(x, y + 4, 2, 9, SSD1306_WHITE);
            _pantalla.fillTriangle(x + 9, y + 4, x + 3, y + 8, x + 9, y + 12, SSD1306_WHITE);
            break;

        case Icono::VOL_MAS:
            // altavoz + líneas
            _pantalla.fillRect(x, y + 5, 4, 6, SSD1306_WHITE);
            _pantalla.fillTriangle(x + 4, y + 3, x + 8, y, x + 8, y + 16, SSD1306_WHITE);
            _pantalla.drawCircle(x + 11, y + 8, 3, SSD1306_WHITE);
            _pantalla.drawCircle(x + 11, y + 8, 5, SSD1306_WHITE);
            break;

        case Icono::VOL_MENOS:
            // altavoz sin ondas
            _pantalla.fillRect(x, y + 5, 4, 6, SSD1306_WHITE);
            _pantalla.fillTriangle(x + 4, y + 3, x + 8, y, x + 8, y + 16, SSD1306_WHITE);
            _pantalla.drawLine(x + 10, y + 5, x + 14, y + 11, SSD1306_WHITE);
            _pantalla.drawLine(x + 14, y + 5, x + 10, y + 11, SSD1306_WHITE);
            break;

        case Icono::PLAY_PAUSE:
            // ▶ ‖
            _pantalla.fillTriangle(x, y + 4, x + 6, y + 8, x, y + 12, SSD1306_WHITE);
            _pantalla.fillRect(x + 8, y + 4, 2, 8, SSD1306_WHITE);
            _pantalla.fillRect(x + 11, y + 4, 2, 8, SSD1306_WHITE);
            break;

        case Icono::FULLSCREEN:
            // esquinas apuntando afuera
            _pantalla.drawLine(x, y, x + 4, y, SSD1306_WHITE);
            _pantalla.drawLine(x, y, x, y + 4, SSD1306_WHITE);
            _pantalla.drawLine(x + 9, y, x + 13, y, SSD1306_WHITE);
            _pantalla.drawLine(x + 13, y, x + 13, y + 4, SSD1306_WHITE);
            _pantalla.drawLine(x, y + 12, x + 4, y + 12, SSD1306_WHITE);
            _pantalla.drawLine(x, y + 8, x, y + 12, SSD1306_WHITE);
            _pantalla.drawLine(x + 9, y + 12, x + 13, y + 12, SSD1306_WHITE);
            _pantalla.drawLine(x + 13, y + 8, x + 13, y + 12, SSD1306_WHITE);
            // letras F11
            _pantalla.setTextSize(1);
            _pantalla.setTextColor(SSD1306_WHITE);
            _pantalla.setCursor(x + 3, y + 4);
            _pantalla.print("F11");
            break;

        case Icono::ESCAPE:
            _pantalla.setTextSize(1);
            _pantalla.setTextColor(SSD1306_WHITE);
            _pantalla.setCursor(x, y + 4);
            _pantalla.print("ESC");
            break;

        case Icono::SCROLL_ABA:
            // tres flechas abajo
            for (uint8_t i = 0; i < 3; i++)
            {
                int16_t yy = y + i * 5;
                _pantalla.drawLine(x + 3, yy, x + 6, yy + 3, SSD1306_WHITE);
                _pantalla.drawLine(x + 6, yy + 3, x + 9, yy, SSD1306_WHITE);
            }
            break;

        case Icono::SCROLL_ARR:
            // tres flechas arriba
            for (uint8_t i = 0; i < 3; i++)
            {
                int16_t yy = y + (2 - i) * 5;
                _pantalla.drawLine(x + 3, yy + 3, x + 6, yy, SSD1306_WHITE);
                _pantalla.drawLine(x + 6, yy, x + 9, yy + 3, SSD1306_WHITE);
            }
            break;

        default:
            // punto de espera
            _pantalla.fillCircle(x + 6, y + 8, 2, SSD1306_WHITE);
            break;
        }
    }

    // Dibuja la pizarra completa
    inline void _dibujarPizarra()
    {
        constexpr int16_t PX = 34; // x inicio pizarra
        constexpr int16_t PW = 93; // ancho pizarra
        constexpr int16_t PH = 60; // alto pizarra

        // marco pizarra
        _pantalla.drawRoundRect(PX, 2, PW, PH, 3, SSD1306_WHITE);

        // separador horizontal
        _pantalla.drawFastHLine(PX + 2, 26, PW - 4, SSD1306_WHITE);

        // nombre del gesto (zona superior)
        _pantalla.setTextSize(1);
        _pantalla.setTextColor(SSD1306_WHITE);
        _pantalla.setCursor(PX + 4, 10);
        _pantalla.print(_nombreGesto);

        // ícono de tecla (zona inferior centrada)
        _dibujarIcono(_icono, PX + 38, 32);

        // pie de pizarra (soporte)
        _pantalla.fillRect(PX + 38, 62, 18, 2, SSD1306_WHITE);
        _pantalla.fillRect(PX + 32, 63, 28, 1, SSD1306_WHITE);
    }

    // ════════════════════════════════════════════════════════════
    //  API PÚBLICA
    // ════════════════════════════════════════════════════════════

    // Notifica un nuevo gesto — actualiza pizarra y acelera baile
    inline void mostrarGesto(const char *nombre, Icono icono)
    {
        if (!_disponible)
            return;
        strncpy(_nombreGesto, nombre, 15);
        _nombreGesto[15] = '\0';
        _icono = icono;
        _gestoActivo = true;
        _tiempoGesto = millis();
    }

    // Mostrar error en pantalla
    inline void mostrarError(const char *mensaje)
    {
        if (!_disponible)
            return;
        strncpy(_nombreGesto, mensaje, 15);
        _nombreGesto[15] = '\0';
        _icono = Icono::NINGUNO;
    }

    // Llamar en cada loop() — dibuja el frame correspondiente
    inline void actualizar()
    {
        if (!_disponible)
            return;

        unsigned long ahora = millis();

        // Volver a modo lento si pasó el tiempo del gesto
        if (_gestoActivo && ahora - _tiempoGesto > MS_GESTO_VISIBLE)
        {
            _gestoActivo = false;
            strncpy(_nombreGesto, "Esperando...", 15);
            _icono = Icono::NINGUNO;
        }

        // Velocidad según si hay gesto activo
        unsigned long intervalo = _gestoActivo ? MS_FRAME_RAPIDO : MS_FRAME_LENTO;
        if (ahora - _tiempoFrame < intervalo)
            return;
        _tiempoFrame = ahora;

        // Avanzar frame
        _frame = (_frame + 1) % 4;

        // Dibujar
        _pantalla.clearDisplay();
        _dibujarRobot(_frame);
        _dibujarPizarra();
        _pantalla.display();
    }

} // namespace PantallaOLED