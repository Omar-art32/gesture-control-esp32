#pragma once
#include <Arduino.h>
#include <BleKeyboard.h>
#include "config.h"
#include "gestos.h"
#include "perfiles.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Bluetooth HID
//  Ejecuta la acción del perfil activo para cada gesto
// ═══════════════════════════════════════════════════════════════

namespace BluetoothHID
{

    static BleKeyboard _teclado(BT_NOMBRE, BT_FABRICANTE, BT_BATERIA);

    inline void iniciar()
    {
        _teclado.begin();
        Serial.println("[BT] Esperando conexion BLE...");
    }

    inline bool estaConectado()
    {
        return _teclado.isConnected();
    }

    // Convierte gesto a slot de perfil
    inline Perfiles::Slot slotDeGesto(Gestos::Gesto gesto)
    {
        switch (gesto)
        {
        case Gestos::Gesto::IZQUIERDA:
            return Perfiles::Slot::IZQUIERDA;
        case Gestos::Gesto::DERECHA:
            return Perfiles::Slot::DERECHA;
        case Gestos::Gesto::ARRIBA:
            return Perfiles::Slot::ARRIBA;
        case Gestos::Gesto::ABAJO:
            return Perfiles::Slot::ABAJO;
        case Gestos::Gesto::ACERCAR:
            return Perfiles::Slot::ACERCAR;
        case Gestos::Gesto::ALEJAR:
            return Perfiles::Slot::ALEJAR;
        case Gestos::Gesto::HORARIO:
            return Perfiles::Slot::HORARIO;
        case Gestos::Gesto::ANTIHORARIO:
            return Perfiles::Slot::ANTIHORARIO;
        default:
            return Perfiles::Slot::DERECHA;
        }
    }

    // Envía la combinación de teclas de la acción
    inline void enviarAccion(const Perfiles::Accion &accion)
    {
        if (!estaConectado())
            return;

        // Presionar modificadores
        if (accion.mod1)
            _teclado.press(accion.mod1);
        if (accion.mod2)
            _teclado.press(accion.mod2);
        if (accion.mod3)
            _teclado.press(accion.mod3);

        // Presionar tecla principal
        _teclado.press(accion.tecla);
        delay(50);

        // Soltar todo
        _teclado.releaseAll();
    }

    // Punto de entrada principal — recibe gesto y ejecuta acción del perfil
    inline const Perfiles::Accion &enviarGesto(Gestos::Gesto gesto)
    {
        const Perfiles::Accion &accion = Perfiles::obtenerAccion(slotDeGesto(gesto));
        enviarAccion(accion);
        return accion;
    }

} // namespace BluetoothHID