// ═══════════════════════════════════════════════════════════════
//  GESTOD — app.js
//  WebSocket recibe gesto + modo
//  Botones envían comando setModo al ESP32
// ═══════════════════════════════════════════════════════════════

const MODOS = [
  {
    nombre: "Presentación",
    color: "text-blue-600",
    bgActivo: "bg-blue-50 border-blue-400",
    gestos: [
      { gesto: "→ Derecha", accion: "Siguiente diapositiva", tecla: "→" },
      { gesto: "← Izquierda", accion: "Diapositiva anterior", tecla: "←" },
      { gesto: "↑ Arriba", accion: "Iniciar presentación", tecla: "F5" },
      { gesto: "↓ Abajo", accion: "Pantalla negra", tecla: "B" },
      { gesto: "✦ Acercar", accion: "Reanudar", tecla: "Enter" },
      { gesto: "✧ Alejar", accion: "Terminar presentación", tecla: "Esc" },
      { gesto: "↻ Horario", accion: "Zoom +", tecla: "Ctrl+↑" },
      { gesto: "↺ Antihorario", accion: "Zoom -", tecla: "Ctrl+↓" },
    ],
  },
  {
    nombre: "Sistema",
    color: "text-cyan-600",
    bgActivo: "bg-cyan-50 border-cyan-400",
    gestos: [
      { gesto: "→ Derecha", accion: "Volumen +", tecla: "F2" },
      { gesto: "← Izquierda", accion: "Volumen -", tecla: "F1" },
      { gesto: "↑ Arriba", accion: "Brillo +", tecla: "F6" }, // corregido
      { gesto: "↓ Abajo", accion: "Brillo -", tecla: "F5" }, // corregido
      {
        gesto: "✦ Acercar",
        accion: "Captura de pantalla",
        tecla: "Win+Shift+S",
      },
      { gesto: "✧ Alejar", accion: "Bloquear PC", tecla: "Win+L" },
      { gesto: "↻ Horario", accion: "Escritorio →", tecla: "Ctrl+Win+→" },
      { gesto: "↺ Antihorario", accion: "Escritorio ←", tecla: "Ctrl+Win+←" },
    ],
  },
  {
    nombre: "Música",
    color: "text-pink-600",
    bgActivo: "bg-pink-50 border-pink-400",
    gestos: [
      { gesto: "→ Derecha", accion: "Siguiente canción", tecla: "Ctrl+→" },
      { gesto: "← Izquierda", accion: "Canción anterior", tecla: "Ctrl+←" },
      { gesto: "↑ Arriba", accion: "Volumen +", tecla: "F2" },
      { gesto: "↓ Abajo", accion: "Volumen -", tecla: "F1" },
      { gesto: "✦ Acercar", accion: "Play / Pause", tecla: "Ctrl+P" },
      { gesto: "✧ Alejar", accion: "Stop", tecla: "Ctrl+S" },
      { gesto: "↻ Horario", accion: "Avanzar 10s", tecla: "→" },
      { gesto: "↺ Antihorario", accion: "Retroceder 10s", tecla: "←" },
    ],
  },
  {
    nombre: "Ventanas",
    color: "text-yellow-600",
    bgActivo: "bg-yellow-50 border-yellow-400",
    gestos: [
      { gesto: "→ Derecha", accion: "Alt+Tab →", tecla: "Alt+Tab" },
      { gesto: "← Izquierda", accion: "Alt+Tab ←", tecla: "Alt+Shift+Tab" },
      { gesto: "↑ Arriba", accion: "Maximizar ventana", tecla: "Win+↑" },
      { gesto: "↓ Abajo", accion: "Minimizar ventana", tecla: "Win+↓" },
      { gesto: "✦ Acercar", accion: "Cerrar ventana", tecla: "Alt+F4" },
      { gesto: "✧ Alejar", accion: "Mostrar escritorio", tecla: "Win+D" },
      { gesto: "↻ Horario", accion: "Ventana →", tecla: "Win+→" },
      { gesto: "↺ Antihorario", accion: "Ventana ←", tecla: "Win+←" },
    ],
  },
];

// Gestos que son mensajes internos del sistema, no acciones del usuario
const GESTOS_SISTEMA = new Set(["Esperando...", "Modo cambiado"]);

// ── Store principal Alpine.js ─────────────────────────────────────
function gestod() {
  return {
    gestoActivo: "Esperando...",
    btConectado: false,
    wsConectado: false,
    modoActivo: 0,
    ultimaActualizacion: "--:--:--",
    contadorGestos: 0,
    historial: [],
    modos: MODOS,

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
    // Formato: { gesto, bt, modo, modoNombre }
    _procesarMensaje(datos) {
      let payload;
      try {
        payload = JSON.parse(datos);
      } catch {
        return;
      }

      this.btConectado = payload.bt ?? false;
      this.modoActivo = payload.modo ?? 0; // siempre viene del servidor (fuente de verdad)

      const nombre = payload.gesto ?? "Esperando...";
      if (!GESTOS_SISTEMA.has(nombre)) {
        // Es un gesto real del usuario → actualizar UI e historial
        this.gestoActivo = nombre;
        this.contadorGestos++;
        this.ultimaActualizacion = this._horaActual();
        this.historial.push({
          gesto: nombre,
          modo: MODOS[this.modoActivo]?.nombre ?? "",
          hora: this._horaActual(),
        });
        if (this.historial.length > 50) this.historial.shift();
      } else if (nombre === "Esperando...") {
        this.gestoActivo = "Esperando...";
      }
      // 'Modo cambiado' solo actualiza modoActivo (ya hecho arriba), sin tocar el historial
    },

    // ── Cambiar modo desde la web ─────────────────────────────
    cambiarModo(indice) {
      if (!this._socket || this._socket.readyState !== WebSocket.OPEN) return;
      this._socket.send(JSON.stringify({ cmd: "setModo", modo: indice }));
      // NO actualizamos modoActivo aquí — esperamos la confirmación del servidor
      // que llegará vía _procesarMensaje con el broadcast de todos los clientes
    },

    // ── Helpers ───────────────────────────────────────────────
    gestosDelModoActivo() {
      return MODOS[this.modoActivo]?.gestos ?? [];
    },

    _horaActual() {
      return new Date().toLocaleTimeString("es-MX", {
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit",
      });
    },
  };
}
