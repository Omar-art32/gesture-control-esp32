#include <Arduino.h>

// ── Módulos del proyecto ─────────────────────────────────────────
#include "config.h"
#include "led_rgb.h"
#include "pantalla_oled.h"
#include "gestos.h"
#include "bluetooth_hid.h"
#include "servidor_web.h"

// ═══════════════════════════════════════════════════════════════
//  GESTOD — main.cpp
//
//  Este archivo solo orquesta los módulos.
//  Toda la lógica vive en include/*.h
//
//  Flujo:
//    1. setup() inicializa cada módulo en orden
//    2. loop() lee el sensor, actualiza estado y notifica
// ═══════════════════════════════════════════════════════════════

// ── Estado global mínimo ──────────────────────────────────────────
static Gestos::Gesto  _gestoActual      = Gestos::Gesto::NINGUNO;
static bool           _btConectadoPrev  = false;   // para detectar cambios de estado

// ── Helpers ───────────────────────────────────────────────────────

// Actualiza OLED, RGB y WebSocket con el estado actual
void actualizarSalidas(Gestos::Gesto gesto) {
    const char* nombre      = Gestos::nombreGesto(gesto);
    bool        btConectado = BluetoothHID::estaConectado();
    String      ipAP        = ServidorWeb::obtenerIP();

    PantallaOLED::mostrarEstado(nombre, btConectado, ipAP.c_str());
    ServidorWeb::notificarGesto(nombre, btConectado);

    // Color del LED según gesto o estado BT
    if (gesto != Gestos::Gesto::NINGUNO) {
        LedRGB::parpadear(LedRGB::COLOR_GESTO, 1, 80);
    }

    LedRGB::establecerColor(btConectado ? LedRGB::COLOR_CONECTADO
                                        : LedRGB::COLOR_ESPERANDO);
}

// ── setup() ───────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    Serial.println("\n══ GESTOD iniciando ══");

    LedRGB::iniciar();
    LedRGB::establecerColor(LedRGB::AMARILLO);   // señal visual: inicializando

    bool oledOk = PantallaOLED::iniciar();
    if (!oledOk) {
        Serial.println("[!] OLED no disponible — continuando sin pantalla");
    }

    bool sensorOk = Gestos::iniciar();
    if (!sensorOk) {
        PantallaOLED::mostrarError("Sensor PAJ7620");
        LedRGB::parpadear(LedRGB::ROJO, 5);
    }

    bool webOk = ServidorWeb::iniciar();
    if (!webOk) {
        PantallaOLED::mostrarError("LittleFS / WiFi");
        LedRGB::parpadear(LedRGB::ROJO, 5);
    }

    BluetoothHID::iniciar();

    // Estado inicial en pantalla
    actualizarSalidas(Gestos::Gesto::NINGUNO);
    Serial.println("══ GESTOD listo ══\n");
}

// ── loop() ────────────────────────────────────────────────────────
void loop() {
    Gestos::Gesto gesto = Gestos::leer();

    if (gesto != Gestos::Gesto::NINGUNO) {
        _gestoActual = gesto;

        Serial.printf("[Gesto] %s\n", Gestos::nombreGesto(gesto));

        BluetoothHID::enviarGesto(gesto);
        actualizarSalidas(gesto);
    }

    // Notificar cambio de estado BT sin esperar un gesto
    bool btAhora = BluetoothHID::estaConectado();
    if (btAhora != _btConectadoPrev) {
        _btConectadoPrev = btAhora;
        actualizarSalidas(_gestoActual);

        Serial.printf("[BT] %s\n", btAhora ? "Conectado" : "Desconectado");
    }
}