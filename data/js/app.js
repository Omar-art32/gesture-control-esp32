// ═══════════════════════════════════════════════════════════════
//  GESTOD — app.js
// ═══════════════════════════════════════════════════════════════

// ── Tablas de gestos por modo ─────────────────────────────────
const GESTOS_PRESENTACION = {
  "PPT / LO": [
    { gesto: "← Izquierda",  accion: "Diapositiva anterior",  tecla: "←"           },
    { gesto: "→ Derecha",    accion: "Siguiente diapositiva",  tecla: "→"           },
    { gesto: "↑ Arriba",     accion: "Iniciar presentación",   tecla: "F5"          },
    { gesto: "↓ Abajo",      accion: "Pantalla negra",         tecla: "B"           },
    { gesto: "✦ Acercar",    accion: "Reanudar",               tecla: "Enter"       },
    { gesto: "✧ Alejar",     accion: "Terminar presentación",  tecla: "Esc"         },
    { gesto: "↻ Horario",    accion: "Zoom +",                 tecla: "Ctrl+="      },
    { gesto: "↺ Antihorario",accion: "Zoom -",                 tecla: "Ctrl+-"      },
  ],
  "GOOGLE": [
    { gesto: "← Izquierda",  accion: "Diapositiva anterior",   tecla: "←"              },
    { gesto: "→ Derecha",    accion: "Siguiente diapositiva",   tecla: "→"              },
    { gesto: "↑ Arriba",     accion: "Iniciar presentación",    tecla: "Ctrl+Shift+F5"  },
    { gesto: "↓ Abajo",      accion: "Pantalla negra",          tecla: "B"              },
    { gesto: "✦ Acercar",    accion: "Reanudar",                tecla: "Enter"          },
    { gesto: "✧ Alejar",     accion: "Terminar presentación",   tecla: "Esc"            },
    { gesto: "↻ Horario",    accion: "Zoom +",                  tecla: "Ctrl+="         },
    { gesto: "↺ Antihorario",accion: "Zoom -",                  tecla: "Ctrl+-"         },
  ],
  "CANVA": [
    { gesto: "← Izquierda",  accion: "Diapositiva anterior",   tecla: "←"              },
    { gesto: "→ Derecha",    accion: "Siguiente diapositiva",   tecla: "→"              },
    { gesto: "↑ Arriba",     accion: "Iniciar presentación",    tecla: "Ctrl+Alt+P"     },
    { gesto: "↓ Abajo",      accion: "Ver notas (presenter)",   tecla: "Alt+P"          },
    { gesto: "✦ Acercar",    accion: "Reanudar",                tecla: "Enter"          },
    { gesto: "✧ Alejar",     accion: "Terminar presentación",   tecla: "Esc"            },
    { gesto: "↻ Horario",    accion: "Zoom +",                  tecla: "Ctrl+="         },
    { gesto: "↺ Antihorario",accion: "Zoom -",                  tecla: "Ctrl+-"         },
  ],
};

const MODOS = [
  {
    nombre: "Presentación",
    color: "text-blue-600",
    bgActivo: "bg-blue-50 border-blue-400",
    // gestos se resuelven dinámicamente según plataforma activa
    gestos: GESTOS_PRESENTACION["PPT / LO"],
  },
  {
    nombre: "Sistema",
    color: "text-cyan-600",
    bgActivo: "bg-cyan-50 border-cyan-400",
    gestos: [
      { gesto: "← Izquierda",  accion: "Volumen -",             tecla: "F1"          },
      { gesto: "→ Derecha",    accion: "Volumen +",             tecla: "F2"          },
      { gesto: "↑ Arriba",     accion: "Brillo +",              tecla: "F6"          },
      { gesto: "↓ Abajo",      accion: "Brillo -",              tecla: "F5"          },
      { gesto: "✦ Acercar",    accion: "Captura de pantalla",   tecla: "Win+Shift+S" },
      { gesto: "✧ Alejar",     accion: "Bloquear PC",           tecla: "Win+L"       },
      { gesto: "↻ Horario",    accion: "Escritorio →",          tecla: "Ctrl+Win+→"  },
      { gesto: "↺ Antihorario",accion: "Escritorio ←",          tecla: "Ctrl+Win+←"  },
    ],
  },
  {
    nombre: "Música",
    color: "text-pink-600",
    bgActivo: "bg-pink-50 border-pink-400",
    gestos: [
      { gesto: "← Izquierda",  accion: "Canción anterior",      tecla: "⏮ HID"      },
      { gesto: "→ Derecha",    accion: "Siguiente canción",      tecla: "⏭ HID"      },
      { gesto: "↑ Arriba",     accion: "Volumen +",              tecla: "🔊 HID"      },
      { gesto: "↓ Abajo",      accion: "Volumen -",              tecla: "🔉 HID"      },
      { gesto: "✦ Acercar",    accion: "Play / Pause",           tecla: "⏯ HID"      },
      { gesto: "✧ Alejar",     accion: "Stop",                   tecla: "⏹ HID"      },
      { gesto: "↻ Horario",    accion: "Avanzar",                tecla: "→"           },
      { gesto: "↺ Antihorario",accion: "Retroceder",             tecla: "←"           },
    ],
  },
  {
    nombre: "Ventanas",
    color: "text-yellow-600",
    bgActivo: "bg-yellow-50 border-yellow-400",
    gestos: [
      { gesto: "← Izquierda",  accion: "Alt+Tab ←",             tecla: "Alt+Shift+Tab" },
      { gesto: "→ Derecha",    accion: "Alt+Tab →",             tecla: "Alt+Tab"       },
      { gesto: "↑ Arriba",     accion: "Maximizar ventana",     tecla: "Win+↑"         },
      { gesto: "↓ Abajo",      accion: "Minimizar ventana",     tecla: "Win+↓"         },
      { gesto: "✦ Acercar",    accion: "Cerrar ventana",        tecla: "Alt+F4"        },
      { gesto: "✧ Alejar",     accion: "Mostrar escritorio",    tecla: "Win+D"         },
      { gesto: "↻ Horario",    accion: "Ventana →",             tecla: "Win+→"         },
      { gesto: "↺ Antihorario",accion: "Ventana ←",             tecla: "Win+←"         },
    ],
  },
];

const PLATAFORMAS = ["PPT / LO", "GOOGLE", "CANVA"];

const GESTOS_SISTEMA = new Set(["Esperando...", "Modo cambiado", "Plataforma cambiada"]);

// ── Store principal Alpine.js ─────────────────────────────────
function gestod() {
  return {
    gestoActivo: "Esperando...",
    btConectado: false,
    wsConectado: false,
    modoActivo: 0,
    plataformaActiva: 0,       // solo relevante en modo PRESENTACION
    ultimaActualizacion: "--:--:--",
    contadorGestos: 0,
    historial: [],
    modos: MODOS,
    plataformas: PLATAFORMAS,

    _socket: null,
    _intervaloReconexion: null,

    iniciar() {
      this._conectarWebSocket();
    },

    // ── WebSocket ─────────────────────────────────────────────
    _conectarWebSocket() {
      const url = `ws://${location.hostname}/ws`;
      this._socket = new WebSocket(url);

      this._socket.onopen = () => {
        this.wsConectado = true;
        clearInterval(this._intervaloReconexion);
      };

      this._socket.onmessage = (e) => this._procesarMensaje(e.data);

      this._socket.onclose = () => {
        this.wsConectado = false;
        this._intervaloReconexion = setInterval(() => {
          this._conectarWebSocket();
        }, 3000);
      };
    },

    // ── Procesar mensaje del ESP32 ────────────────────────────
    // { gesto, bt, modo, modoNombre, plataforma, plataformaNombre }
    _procesarMensaje(datos) {
      let payload;
      try { payload = JSON.parse(datos); } catch { return; }

      this.btConectado     = payload.bt          ?? false;
      this.modoActivo      = payload.modo        ?? 0;
      this.plataformaActiva= payload.plataforma  ?? 0;

      const nombre = payload.gesto ?? "Esperando...";
      if (!GESTOS_SISTEMA.has(nombre)) {
        this.gestoActivo = nombre;
        this.contadorGestos++;
        this.ultimaActualizacion = this._horaActual();
        this.historial.push({
          gesto: nombre,
          modo:  this._etiquetaModoActual(),
          hora:  this._horaActual(),
        });
        if (this.historial.length > 50) this.historial.shift();
      } else if (nombre === "Esperando...") {
        this.gestoActivo = "Esperando...";
      }
    },

    // ── Cambiar modo desde la web ─────────────────────────────
    cambiarModo(indice) {
      if (!this._socket || this._socket.readyState !== WebSocket.OPEN) return;
      this._socket.send(JSON.stringify({ cmd: "setModo", modo: indice }));
    },

    // ── Cambiar plataforma desde la web ───────────────────────
    cambiarPlataforma(indice) {
      if (!this._socket || this._socket.readyState !== WebSocket.OPEN) return;
      this._socket.send(JSON.stringify({ cmd: "setPlataforma", plataforma: indice }));
    },

    // ── Gestos del modo/plataforma activos ────────────────────
    gestosDelModoActivo() {
      if (this.modoActivo === 0) {
        // PRESENTACION: devolver tabla de la plataforma activa
        const nombrePlat = PLATAFORMAS[this.plataformaActiva] ?? "PPT / LO";
        return GESTOS_PRESENTACION[nombrePlat] ?? GESTOS_PRESENTACION["PPT / LO"];
      }
      return MODOS[this.modoActivo]?.gestos ?? [];
    },

    // ── Etiqueta legible del modo/plataforma actual ───────────
    _etiquetaModoActual() {
      if (this.modoActivo === 0)
        return `Pres. (${PLATAFORMAS[this.plataformaActiva] ?? "PPT"})`;
      return MODOS[this.modoActivo]?.nombre ?? "";
    },

    // ── Helpers ───────────────────────────────────────────────
    _horaActual() {
      return new Date().toLocaleTimeString("es-MX", {
        hour: "2-digit", minute: "2-digit", second: "2-digit",
      });
    },
  };
}