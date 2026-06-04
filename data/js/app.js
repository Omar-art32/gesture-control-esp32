// ═══════════════════════════════════════════════════════
//  GESTOD — app.js
//  Alpine.js store principal
//  WebSocket se conecta al ESP32 en ws://192.168.4.1/ws
// ═══════════════════════════════════════════════════════

// Catálogo completo de gestos con símbolo, acción y tecla enviada
const CATALOGO_GESTOS = [
  {
    nombre:  '<- Izquierda',
    simbolo: '←',
    accion:  'Pista / diapositiva anterior',
    tecla:   'MEDIA_PREV',
  },
  {
    nombre:  '-> Derecha',
    simbolo: '→',
    accion:  'Siguiente pista / diapositiva',
    tecla:   'MEDIA_NEXT',
  },
  {
    nombre:  '^ Arriba',
    simbolo: '↑',
    accion:  'Subir volumen',
    tecla:   'MEDIA_VOL+',
  },
  {
    nombre:  'v Abajo',
    simbolo: '↓',
    accion:  'Bajar volumen',
    tecla:   'MEDIA_VOL-',
  },
  {
    nombre:  '~ Wave',
    simbolo: '〜',
    accion:  'Play / Pause',
    tecla:   'MEDIA_PLAY',
  },
  {
    nombre:  '* Acercar',
    simbolo: '✦',
    accion:  'Pantalla completa',
    tecla:   'F11',
  },
  {
    nombre:  '* Alejar',
    simbolo: '✧',
    accion:  'Salir / Escape',
    tecla:   'ESC',
  },
  {
    nombre:  '@ Horario',
    simbolo: '↻',
    accion:  'Scroll hacia abajo',
    tecla:   '↓ ↓ ↓',
  },
  {
    nombre:  '@ Antihorario',
    simbolo: '↺',
    accion:  'Scroll hacia arriba',
    tecla:   '↑ ↑ ↑',
  },
];

// Mapa para búsqueda rápida por nombre
const MAPA_GESTOS = Object.fromEntries(
  CATALOGO_GESTOS.map(g => [g.nombre, g])
);

// ── Store principal de Alpine.js ──────────────────────────
function gestod() {
  return {
    // Estado reactivo
    gestoActivo:       'Esperando...',
    btConectado:       false,
    wsConectado:       false,
    ultimaActualizacion: '--:--:--',
    contadorGestos:    0,
    historial:         [],
    gestos:            CATALOGO_GESTOS,

    // Conexión WebSocket
    _socket: null,
    _intervaloReconexion: null,

    // ── Ciclo de vida ──────────────────────────────────────
    iniciar() {
      this._conectarWebSocket();
    },

    // ── WebSocket ──────────────────────────────────────────
    _conectarWebSocket() {
      const url = `ws://${location.hostname}/ws`;

      this._socket = new WebSocket(url);

      this._socket.onopen = () => {
        this.wsConectado = true;
        clearInterval(this._intervaloReconexion);
        console.log('[WS] Conectado a', url);
      };

      this._socket.onmessage = (evento) => {
        this._procesarMensaje(evento.data);
      };

      this._socket.onclose = () => {
        this.wsConectado = false;
        console.warn('[WS] Desconectado — reintentando en 3s...');
        this._intervaloReconexion = setInterval(() => {
          this._conectarWebSocket();
        }, 3000);
      };

      this._socket.onerror = (error) => {
        console.error('[WS] Error:', error);
      };
    },

    // ── Procesar mensaje del ESP32 ─────────────────────────
    // Formato esperado: { "gesto": "<- Izquierda", "bt": true }
    _procesarMensaje(datos) {
      let payload;
      try {
        payload = JSON.parse(datos);
      } catch {
        console.warn('[WS] Mensaje no válido:', datos);
        return;
      }

      const nombreGesto = payload.gesto ?? 'Esperando...';
      const btEstado    = payload.bt   ?? false;

      // Actualizar estado
      this.btConectado = btEstado;

      if (nombreGesto !== 'Esperando...') {
        this.gestoActivo = nombreGesto;
        this.contadorGestos++;
        this.ultimaActualizacion = this._horaActual();

        // Agregar al historial
        this.historial.push({
          gesto:  nombreGesto,
          accion: this.accionGesto(nombreGesto),
          hora:   this._horaActual(),
        });

        // Mantener máximo 50 entradas
        if (this.historial.length > 50) {
          this.historial.shift();
        }
      } else {
        this.gestoActivo = 'Esperando...';
      }
    },

    // ── Helpers de UI ─────────────────────────────────────
    simboloGesto(nombre) {
      return MAPA_GESTOS[nombre]?.simbolo ?? '·';
    },

    accionGesto(nombre) {
      return MAPA_GESTOS[nombre]?.accion ?? 'Sin acción asignada';
    },

    _horaActual() {
      return new Date().toLocaleTimeString('es-MX', {
        hour:   '2-digit',
        minute: '2-digit',
        second: '2-digit',
      });
    },
  };
}