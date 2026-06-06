#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "config.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Servidor Web
//  WiFi Access Point + archivos estáticos + WebSocket
// ═══════════════════════════════════════════════════════════════

namespace ServidorWeb
{

    static AsyncWebServer _servidor(PUERTO_WEB);
    static AsyncWebSocket _ws("/ws");

    static void _onWebSocketEvento(
        AsyncWebSocket *servidor,
        AsyncWebSocketClient *cliente,
        AwsEventType tipo,
        void *argumento,
        uint8_t *datos,
        size_t longitud)
    {
        if (tipo == WS_EVT_CONNECT)
        {
            Serial.printf("[WS] Cliente #%u conectado\n", cliente->id());
        }
        else if (tipo == WS_EVT_DISCONNECT)
        {
            Serial.printf("[WS] Cliente #%u desconectado\n", cliente->id());
        }
    }

    inline bool iniciar()
    {
        if (!LittleFS.begin())
        {
            Serial.println("[Web] Error al montar LittleFS");
            return false;
        }

        WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
        Serial.printf("[WiFi] AP activo — SSID: %s  IP: %s\n",
                      WIFI_SSID,
                      WiFi.softAPIP().toString().c_str());

        _ws.onEvent(_onWebSocketEvento);
        _servidor.addHandler(&_ws);

        // Servir cada archivo explícitamente — sin búsqueda de .gz
        _servidor.on("/", HTTP_GET, [](AsyncWebServerRequest *req)
                     { req->send(LittleFS, "/index.html", "text/html"); });
        _servidor.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *req)
                     { req->send(LittleFS, "/index.html", "text/html"); });
        _servidor.on("/css/styles.css", HTTP_GET, [](AsyncWebServerRequest *req)
                     { req->send(LittleFS, "/css/styles.css", "text/css"); });
        _servidor.on("/js/app.js", HTTP_GET, [](AsyncWebServerRequest *req)
                     { req->send(LittleFS, "/js/app.js", "application/javascript"); });
        _servidor.on("/js/tailwind.min.js", HTTP_GET, [](AsyncWebServerRequest *req)
                     { req->send(LittleFS, "/js/tailwind.min.js", "application/javascript"); });
        _servidor.on("/js/alpine.min.js", HTTP_GET, [](AsyncWebServerRequest *req)
                     { req->send(LittleFS, "/js/alpine.min.js", "application/javascript"); });

        _servidor.onNotFound([](AsyncWebServerRequest *req)
                             { req->send(404, "text/plain", "No encontrado"); });

        _servidor.begin();
        Serial.println("[Web] Servidor HTTP iniciado en puerto 80");
        return true;
    }

    inline void notificarGesto(const char *nombreGesto, bool btConectado)
    {
        _ws.cleanupClients();
        if (_ws.count() == 0)
            return;

        JsonDocument doc;
        doc["gesto"] = nombreGesto;
        doc["bt"] = btConectado;

        String json;
        serializeJson(doc, json);
        _ws.textAll(json);
    }

    inline String obtenerIP()
    {
        return WiFi.softAPIP().toString();
    }

} // namespace ServidorWeb