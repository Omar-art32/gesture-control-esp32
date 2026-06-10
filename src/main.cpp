#include <Arduino.h>
#include "config.h"
#include "led_rgb.h"
#include "pantalla_oled.h"
#include "gestos.h"
#include "perfiles.h"
#include "bluetooth_hid.h"
#include "servidor_web.h"

// ═══════════════════════════════════════════════════════════════
//  GESTOD — main.cpp
// ═══════════════════════════════════════════════════════════════

static Gestos::Gesto _gestoActual = Gestos::Gesto::NINGUNO;
static bool _btConectadoPrev = false;

// ── Muestra el modo activo en OLED y LED ──────────────────────────
void mostrarCambioModo()
{
    LedRGB::parpadear(Perfiles::colorModoActual(), 3, 100);
    LedRGB::establecerColor(Perfiles::colorModoActual());

    PantallaOLED::mostrarGesto(
        Perfiles::nombreModoActual(),
        PantallaOLED::Icono::NINGUNO);

    Serial.printf("[Modo] %s\n", Perfiles::nombreModoActual());
    ServidorWeb::notificarGesto("Modo cambiado",
                                BluetoothHID::estaConectado());
}

// ── Procesa un gesto detectado ────────────────────────────────────
void procesarGesto(Gestos::Gesto gesto)
{
    _gestoActual = gesto;

    // Wave = cambiar modo
    if (gesto == Gestos::Gesto::WAVE)
    {
        Perfiles::siguienteModo();
        mostrarCambioModo();
        return;
    }

    // Resto de gestos → ejecutar acción del perfil activo
    const Perfiles::Accion &accion = BluetoothHID::enviarGesto(gesto);

    Serial.printf("[Gesto] %s | Modo: %s | Accion: %s\n",
                  Gestos::nombreGesto(gesto),
                  Perfiles::nombreModoActual(),
                  accion.nombre);

    PantallaOLED::mostrarGesto(accion.nombre, PantallaOLED::Icono::NINGUNO);

    LedRGB::parpadear(Perfiles::colorModoActual(), 1, 80);
    LedRGB::establecerColor(Perfiles::colorModoActual());

    ServidorWeb::notificarGesto(accion.nombre,
                                BluetoothHID::estaConectado());
}

// ── setup() ───────────────────────────────────────────────────────
void setup()
{
    Serial.begin(115200);
    Serial.println("\n══ GESTOD iniciando ══");

    LedRGB::iniciar();
    LedRGB::establecerColor(LedRGB::AMARILLO);

    bool oledOk = PantallaOLED::iniciar();
    if (!oledOk)
        Serial.println("[!] OLED no disponible");

    bool sensorOk = Gestos::iniciar();
    if (!sensorOk)
    {
        PantallaOLED::mostrarError("PAJ7620 ERROR");
        LedRGB::parpadear(LedRGB::ROJO, 5);
    }

    bool webOk = ServidorWeb::iniciar();
    if (!webOk)
    {
        PantallaOLED::mostrarError("WiFi ERROR");
        LedRGB::parpadear(LedRGB::ROJO, 5);
    }

    BluetoothHID::iniciar();

    // Estado inicial — modo 0 PRESENTACION
    LedRGB::establecerColor(Perfiles::colorModoActual());

    Serial.println("══ GESTOD listo ══\n");
}

// ── loop() ────────────────────────────────────────────────────────
void loop()
{
    PantallaOLED::actualizar();

    Gestos::Gesto gesto = Gestos::leer();
    if (gesto != Gestos::Gesto::NINGUNO)
    {
        procesarGesto(gesto);
    }

    bool btAhora = BluetoothHID::estaConectado();
    if (btAhora != _btConectadoPrev)
    {
        _btConectadoPrev = btAhora;
        Serial.printf("[BT] %s\n", btAhora ? "Conectado" : "Desconectado");
        LedRGB::establecerColor(btAhora ? Perfiles::colorModoActual()
                                        : LedRGB::COLOR_ESPERANDO);
    }
}