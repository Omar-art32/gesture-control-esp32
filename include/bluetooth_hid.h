#pragma once
#include <Arduino.h>
#include <BleKeyboard.h>
#include "config.h"
#include "gestos.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Bluetooth HID
//  Librería: t-vk/ESP32 BLE Keyboard @ ^0.3.2
//  Mapea cada gesto a una tecla o combinación de teclado
// ═══════════════════════════════════════════════════════════════

namespace BluetoothHID {

    static BleKeyboard _teclado(BT_NOMBRE, BT_FABRICANTE, BT_BATERIA);

    // ── Inicialización ────────────────────────────────────────────
    inline void iniciar() {
        _teclado.begin();
        Serial.println("[BT] Esperando conexión BLE...");
    }

    inline bool estaConectado() {
        return _teclado.isConnected();
    }

    // ── Mapeo gesto → acción de teclado ──────────────────────────
    //
    //  Izquierda     → Anterior pista / diapositiva
    //  Derecha       → Siguiente pista / diapositiva
    //  Arriba        → Volumen +
    //  Abajo         → Volumen -
    //  Wave          → Play / Pause
    //  Acercar       → Pantalla completa (F11)
    //  Alejar        → Escape
    //  Horario       → Scroll abajo
    //  Antihorario   → Scroll arriba
    //
    inline void enviarGesto(Gestos::Gesto gesto) {
        if (!estaConectado()) return;

        switch (gesto) {
            case Gestos::Gesto::IZQUIERDA:
                _teclado.write(KEY_MEDIA_PREVIOUS_TRACK);
                break;

            case Gestos::Gesto::DERECHA:
                _teclado.write(KEY_MEDIA_NEXT_TRACK);
                break;

            case Gestos::Gesto::ARRIBA:
                _teclado.write(KEY_MEDIA_VOLUME_UP);
                break;

            case Gestos::Gesto::ABAJO:
                _teclado.write(KEY_MEDIA_VOLUME_DOWN);
                break;

            case Gestos::Gesto::WAVE:
                _teclado.write(KEY_MEDIA_PLAY_PAUSE);
                break;

            case Gestos::Gesto::ACERCAR:
                _teclado.write(KEY_F11);   // pantalla completa
                break;

            case Gestos::Gesto::ALEJAR:
                _teclado.write(KEY_ESC);
                break;

            case Gestos::Gesto::HORARIO:
                // Scroll abajo = flecha abajo 3 veces
                for (uint8_t i = 0; i < 3; i++) {
                    _teclado.write(KEY_DOWN_ARROW);
                }
                break;

            case Gestos::Gesto::ANTIHORARIO:
                for (uint8_t i = 0; i < 3; i++) {
                    _teclado.write(KEY_UP_ARROW);
                }
                break;

            default:
                break;
        }
    }

} // namespace BluetoothHID