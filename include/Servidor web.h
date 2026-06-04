#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "config.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Servidor Web
//  Crea red WiFi propia (Access Point) y sirve data/ por HTTP
//  WebSocket en /ws para enviar el gesto activo en tiempo real
// ═══════════════════════════════════════════════════════════════

namespace ServidorWeb {

    static AsyncWebServer _servidor(PUERTO_WEB);
    static AsyncWebSocket _ws("/ws");

    // ── Callback de eventos WebSocket ─────────────────────────────
    static void _onWebSocketEvento(
        AsyncWebSocket* servidor,
        AsyncWebSocketClient* cliente,
        AwsEventType tipo,
        void* argumento,
        uint8_t* datos,
        size_t longitud
    ) {
        if (tipo == WS_EVT_CONNECT) {
            Serial.printf("[WS] Cliente #%u conectado\n", cliente->id());
        } else if (tipo == WS_EVT_DISCONNECT) {
            Serial.printf("[WS] Cliente #%u desconectado\n", cliente->id());
        }
    }

    // ── Inicialización ────────────────────────────────────────────
    inline bool iniciar() {
        // Montar LittleFS
        if (!LittleFS.begin()) {
            Serial.println("[Web] Error al montar LittleFS");
            return false;
        }

        // Levantar Access Point
        WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
        Serial.printf("[WiFi] AP activo — SSID: %s  IP: %s\n",
                      WIFI_SSID,
                      WiFi.softAPIP().toString().c_str());

        // WebSocket
        _ws.onEvent(_onWebSocketEvento);
        _servidor.addHandler(&_ws);

        // Archivos estáticos desde LittleFS
        _servidor.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

        // 404 genérico
        _servidor.onNotFound([](AsyncWebServerRequest* solicitud) {
            solicitud->send(404, "text/plain", "No encontrado");
        });

        _servidor.begin();
        Serial.println("[Web] Servidor HTTP iniciado en puerto 80");
        return true;
    }

    // ── Enviar gesto activo a todos los clientes WebSocket ────────
    inline void notificarGesto(const char* nombreGesto, bool btConectado) {
        // Limpiar clientes desconectados periódicamente
        _ws.cleanupClients();

        if (_ws.count() == 0) return;

        // Serializar como JSON: {"gesto":"<- Izquierda","bt":true}
        JsonDocument doc;
        doc["gesto"] = nombreGesto;
        doc["bt"]    = btConectado;

        String json;
        serializeJson(doc, json);
        _ws.textAll(json);
    }

    // ── IP del Access Point para mostrar en OLED ──────────────────
    inline String obtenerIP() {
        return WiFi.softAPIP().toString();
    }

} // namespace ServidorWeb