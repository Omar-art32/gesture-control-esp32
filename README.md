# GESTOD — Control de Gestos con ESP32

> Control inalámbrico de la computadora mediante gestos de mano. El ESP32 detecta gestos con el sensor PAJ7620 y los envía como teclado Bluetooth HID a la PC. Incluye interfaz web informativa servida desde el propio ESP32.

---

## ¿Cómo funciona?

```
Mano → PAJ7620 → ESP32 → Bluetooth HID → PC
                   ↓
              WiFi Access Point
                   ↓
           Web informativa (celular/tablet)
```

El ESP32 crea su propia red WiFi `GESTOD` — sin depender de un router externo. Conectas tu celular a esa red para ver la guía de gestos en tiempo real. Simultáneamente, el ESP32 está emparejado con la PC por BLE y actúa como teclado inalámbrico.

---

## Hardware

| Componente | Descripción |
|---|---|
| ESP32 DevKit v1 (30 pines) | Microcontrolador principal |
| CJMCU-7620 (PAJ7620) | Sensor de gestos por infrarojo |
| OLED SSD1306 128x64 | Pantalla — muestra gesto activo e IP |
| LED RGB cátodo común | Indicador visual de estado |
| 3x resistencia 100Ω | Una por cada pin de color del RGB |

---

## Conexiones

### I²C — OLED y PAJ7620 comparten el mismo bus

| Componente | Pin | ESP32 GPIO |
|---|---|---|
| OLED + PAJ7620 | VCC | 3.3V |
| OLED + PAJ7620 | GND | GND |
| OLED + PAJ7620 | SCL | GPIO 22 |
| OLED + PAJ7620 | SDA | GPIO 21 |

> Direcciones I²C: OLED en `0x3C`, PAJ7620 en `0x73`

### LED RGB (cátodo común: orden de pines `- G R B`)

| Pin RGB | ESP32 GPIO | Resistencia |
|---|---|---|
| `-` (cátodo) | GND | — |
| `G` (verde) | GPIO 26 | 100Ω |
| `R` (rojo) | GPIO 25 | 100Ω |
| `B` (azul) | GPIO 27 | 100Ω |

---

## Gestos y acciones

| Gesto | Acción en la PC |
|---|---|
| ← Izquierda | Pista / diapositiva anterior |
| → Derecha | Siguiente pista / diapositiva |
| ↑ Arriba | Subir volumen |
| ↓ Abajo | Bajar volumen |
| 〜 Wave | Play / Pause |
| ✦ Acercar | Pantalla completa (F11) |
| ✧ Alejar | Escape |
| ↻ Horario | Scroll abajo |
| ↺ Antihorario | Scroll arriba |

---

## Estructura del proyecto

```
gesture-control-esp32/
├── platformio.ini          # Librerías y configuración de compilación
├── include/
│   ├── config.h            # Pines, SSID, constantes — fuente única de verdad
│   ├── led_rgb.h           # Módulo LED RGB con PWM nativo (ledcWrite)
│   ├── pantalla_oled.h     # Módulo OLED SSD1306
│   ├── gestos.h            # Módulo PAJ7620 con debounce
│   ├── bluetooth_hid.h     # Módulo BLE HID teclado
│   └── servidor_web.h      # WiFi AP + ESPAsyncWebServer + WebSocket
├── src/
│   └── main.cpp            # Orquesta los módulos — setup() y loop()
└── data/                   # Se sube al LittleFS del ESP32
    ├── index.html           # Interfaz web con guía de gestos
    ├── css/styles.css
    └── js/app.js            # WebSocket — gesto activo en tiempo real
```

---

## Librerías utilizadas

```ini
t-vk/ESP32 BLE Keyboard @ ^0.3.2
RevEng PAJ7620 @ ^1.4.1
adafruit/Adafruit SSD1306 @ ^2.5.9
adafruit/Adafruit GFX Library @ ^1.11.9
https://github.com/me-no-dev/ESPAsyncWebServer.git
ESP32Async/AsyncTCP @ ^3.3.2
bblanchon/ArduinoJson @ ^7.3.0
```

Todas se descargan automáticamente al compilar con PlatformIO.

---

## Instalación

### 1. Clonar el repositorio

```bash
git clone https://github.com/Omar-art32/gesture-control-esp32.git
cd gesture-control-esp32
```

### 2. Abrir en VS Code con PlatformIO

PlatformIO descarga todas las librerías automáticamente al compilar.

### 3. Subir el sistema de archivos (web)

```
PlatformIO → Project Tasks → Upload Filesystem Image
```

Esto sube la carpeta `data/` al LittleFS del ESP32.

### 4. Compilar y subir el firmware

```
PlatformIO → Upload
```

### 5. Emparejar Bluetooth

En la PC: Ajustes → Bluetooth → Agregar dispositivo → buscar **GESTOD Teclado**

Solo se empareja una vez.

### 6. Conectar a la web

En el celular: conectar a la red WiFi **GESTOD** (contraseña: `gestod123`) y abrir `192.168.4.1`

---

## Indicadores LED RGB

| Color | Estado |
|---|---|
| 🟡 Amarillo | Inicializando |
| 🔵 Azul | Esperando gesto (BT desconectado) |
| 🟢 Verde | BT conectado — listo |
| Parpadeo amarillo | Gesto detectado |
| 🔴 Rojo (parpadeo) | Error de inicialización |

---

## Tecnologías

- **Firmware:** C++ con Arduino framework en PlatformIO
- **Comunicación PC:** Bluetooth Low Energy HID
- **Red propia:** WiFi Access Point (sin router externo)
- **Web:** Tailwind CSS + Alpine.js + WebSocket
- **Sistema de archivos:** LittleFS

---

## Proyecto escolar

Desarrollado como proyecto de control por gestos sin contacto para presentación escolar.

**Alumno:** Omar  
**Hardware:** ESP32 + PAJ7620 + SSD1306 + RGB LED  
**Entorno:** PlatformIO + VS Code