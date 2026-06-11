#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "config.h"
#include "perfiles.h"

// ═══════════════════════════════════════════════════════════════
//  Módulo Servidor Web
//  WebSocket notifica: gesto, bt, modo, modoNombre,
//                      plataforma, plataformaNombre
//  Comandos recibidos:
//    { "cmd": "setModo",       "modo": N }
//    { "cmd": "setPlataforma", "plataforma": N }
// ═══════════════════════════════════════════════════════════════

namespace ServidorWeb
{

    static AsyncWebServer _servidor(PUERTO_WEB);
    static AsyncWebSocket _ws("/ws");

    inline void notificarGesto(const char *nombreGesto, bool btConectado);

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
            JsonDocument doc;
            doc["gesto"]            = "Esperando...";
            doc["bt"]               = false;
            doc["modo"]             = Perfiles::modoActual();
            doc["modoNombre"]       = Perfiles::NOMBRE_MODO[Perfiles::modoActual()];
            doc["plataforma"]       = Perfiles::plataformaActual();
            doc["plataformaNombre"] = Perfiles::NOMBRE_PLATAFORMA[Perfiles::plataformaActual()];
            String json;
            serializeJson(doc, json);
            cliente->text(json);
        }
        else if (tipo == WS_EVT_DISCONNECT)
        {
            Serial.printf("[WS] Cliente #%u desconectado\n", cliente->id());
        }
        else if (tipo == WS_EVT_DATA && longitud > 0)
        {
            String msg = String((char *)datos).substring(0, longitud);
            JsonDocument doc;
            if (deserializeJson(doc, msg) != DeserializationError::Ok) return;

            const char *cmd = doc["cmd"] | "";

            if (strcmp(cmd, "setModo") == 0)
            {
                uint8_t modo = doc["modo"] | 0;
                Perfiles::establecerModo(modo);
                Serial.printf("[Web] Modo → %s\n", Perfiles::nombreModoActual());
                notificarGesto("Modo cambiado", false);
            }
            else if (strcmp(cmd, "setPlataforma") == 0)
            {
                uint8_t plat = doc["plataforma"] | 0;
                Perfiles::establecerPlataforma(plat);
                Serial.printf("[Web] Plataforma → %s\n",
                              Perfiles::NOMBRE_PLATAFORMA[Perfiles::plataformaActual()]);
                notificarGesto("Plataforma cambiada", false);
            }
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
                      WIFI_SSID, WiFi.softAPIP().toString().c_str());

        _ws.onEvent(_onWebSocketEvento);
        _servidor.addHandler(&_ws);

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
        Serial.println("[Web] Servidor HTTP iniciado");
        return true;
    }

    inline void notificarGesto(const char *nombreGesto, bool btConectado)
    {
        _ws.cleanupClients();
        if (_ws.count() == 0) return;

        JsonDocument doc;
        doc["gesto"]            = nombreGesto;
        doc["bt"]               = btConectado;
        doc["modo"]             = Perfiles::modoActual();
        doc["modoNombre"]       = Perfiles::NOMBRE_MODO[Perfiles::modoActual()];
        doc["plataforma"]       = Perfiles::plataformaActual();
        doc["plataformaNombre"] = Perfiles::NOMBRE_PLATAFORMA[Perfiles::plataformaActual()];

        String json;
        serializeJson(doc, json);
        _ws.textAll(json);
    }

    inline String obtenerIP()
    {
        return WiFi.softAPIP().toString();
    }

} // namespace ServidorWeb