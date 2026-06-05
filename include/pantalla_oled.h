#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Pantalla OLED
//  Librería: adafruit/Adafruit SSD1306 @ ^2.5.9
//  Muestra: gesto activo, estado BT, IP del Access Point
// ═══════════════════════════════════════════════════════════════

namespace PantallaOLED {

    // Instancia interna — solo se accede a través de las funciones del módulo
    static Adafruit_SSD1306 _pantalla(OLED_ANCHO, OLED_ALTO, &Wire, -1);
    static bool _disponible = false;

    // ── Inicialización ────────────────────────────────────────────
    inline bool iniciar() {
        Wire.begin(OLED_SDA, OLED_SCL);

        _disponible = _pantalla.begin(SSD1306_SWITCHCAPVCC, OLED_DIRECCION);

        if (!_disponible) {
            Serial.println("[OLED] No se encontró la pantalla en 0x3C");
            return false;
        }

        _pantalla.clearDisplay();
        _pantalla.setTextColor(SSD1306_WHITE);
        _pantalla.setTextSize(1);

        // Pantalla de bienvenida
        _pantalla.setCursor(28, 20);
        _pantalla.println("G E S T O D");
        _pantalla.setCursor(22, 38);
        _pantalla.println("Iniciando...");
        _pantalla.display();

        return true;
    }

    // ── Pantalla principal durante el uso ─────────────────────────
    //
    //  ┌──────────────────────────┐
    //  │ GESTOD            BT: ✓ │  ← línea 1: título + estado BT
    //  │ ─────────────────────── │
    //  │  Gesto:                 │  ← línea 3
    //  │  ← Izquierda            │  ← línea 4: gesto activo grande
    //  │                         │
    //  │  192.168.4.1            │  ← línea 6: IP del AP
    //  └──────────────────────────┘
    inline void mostrarEstado(const char* nombreGesto, bool bluetoothConectado, const char* ipAP) {
        if (!_disponible) return;

        _pantalla.clearDisplay();

        // Línea 1: encabezado
        _pantalla.setTextSize(1);
        _pantalla.setCursor(0, 0);
        _pantalla.print("GESTOD");

        _pantalla.setCursor(80, 0);
        _pantalla.print("BT:");
        _pantalla.print(bluetoothConectado ? " OK" : " --");

        // Separador horizontal
        _pantalla.drawFastHLine(0, 10, OLED_ANCHO, SSD1306_WHITE);

        // Etiqueta gesto
        _pantalla.setTextSize(1);
        _pantalla.setCursor(0, 14);
        _pantalla.print("Gesto:");

        // Nombre del gesto en tamaño grande
        _pantalla.setTextSize(2);
        _pantalla.setCursor(0, 26);
        _pantalla.print(nombreGesto);

        // IP del Access Point al fondo
        _pantalla.setTextSize(1);
        _pantalla.setCursor(0, 56);
        _pantalla.print(ipAP);

        _pantalla.display();
    }

    // ── Mensaje de error centrado ─────────────────────────────────
    inline void mostrarError(const char* mensaje) {
        if (!_disponible) return;

        _pantalla.clearDisplay();
        _pantalla.setTextSize(1);
        _pantalla.setCursor(0, 24);
        _pantalla.println("! ERROR !");
        _pantalla.setCursor(0, 38);
        _pantalla.println(mensaje);
        _pantalla.display();
    }

    // ── Apagar pantalla ───────────────────────────────────────────
    inline void apagar() {
        if (!_disponible) return;
        _pantalla.clearDisplay();
        _pantalla.display();
        _pantalla.ssd1306_command(SSD1306_DISPLAYOFF);
    }

} // namespace PantallaOLED