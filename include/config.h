#pragma once

// ═══════════════════════════════════════════════════════════════
//  GESTOD — Configuración central del proyecto
//  Cambia aquí los pines y parámetros sin tocar el código fuente
// ═══════════════════════════════════════════════════════════════

// ── WiFi Access Point ────────────────────────────────────────────
constexpr char WIFI_SSID[]     = "GESTOD";
constexpr char WIFI_PASSWORD[] = "gestod123";   // mínimo 8 caracteres

// ── Bluetooth ────────────────────────────────────────────────────
constexpr char BT_NOMBRE[]        = "GESTOD Teclado";
constexpr char BT_FABRICANTE[]    = "GESTOD";
constexpr uint8_t BT_BATERIA      = 100;

// ── Pines LED RGB (cátodo común: HIGH = encendido) ───────────────
constexpr uint8_t PIN_ROJO   = 25;
constexpr uint8_t PIN_VERDE  = 26;
constexpr uint8_t PIN_AZUL   = 27;

// Canales LEDC (PWM por hardware del ESP32, uno por pin)
constexpr uint8_t CANAL_ROJO  = 0;
constexpr uint8_t CANAL_VERDE = 1;
constexpr uint8_t CANAL_AZUL  = 2;

constexpr uint32_t PWM_FRECUENCIA  = 5000;   // Hz
constexpr uint8_t  PWM_RESOLUCION  = 8;      // bits → 0–255

// ── Pines OLED (I²C) ─────────────────────────────────────────────
constexpr uint8_t OLED_SDA       = 21;
constexpr uint8_t OLED_SCL       = 22;
constexpr uint8_t OLED_ANCHO     = 128;
constexpr uint8_t OLED_ALTO      = 64;
constexpr uint8_t OLED_DIRECCION = 0x3C;   // dirección I²C estándar SSD1306

// ── Sensor PAJ7620 (I²C, misma línea que OLED) ───────────────────
// El PAJ7620 usa dirección 0x73 — la librería lo maneja sola

// ── Debounce del sensor ───────────────────────────────────────────
constexpr unsigned long MS_DEBOUNCE = 200;   // milisegundos entre gestos

// ── Servidor web ─────────────────────────────────────────────────
constexpr uint16_t PUERTO_WEB = 80;