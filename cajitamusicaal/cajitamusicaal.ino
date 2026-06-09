
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_PWMServoDriver.h>
#include "laufey.h"
#include "bitmapssss.h"
Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);
WebServer server(80);

const char* ssid     = "INFINITUMFA7";
const char* password = "qGxuz7ZErx";

#define SERVO_MIN 150
#define SERVO_MAX 600
int  TECLAS[12]        = {554,622,698,740,831,932,1047,1109,1245,1397,1480,1661};
String NOMBRES_TECLAS[12] = {
  "Db5","Eb5","F5","Gb5","Ab5","Bb5",
  "C6","Db6","Eb6","F6","Gb6","Ab6"
};

struct Evento { int tecla; int duracion; };

#include "Laufey_Dreamer_eventos.h"
#include "Laufey_Second_Best_eventos.h"
#include "Laufey_Haunted_eventos.h"
#include "Laufey_Must_Be_Love_eventos.h"
#include "Laufey_While_You_Were_Sleeping_eventos.h"
#include "Laufey_Lovesick_eventos.h"
#include "Laufey_California_and_Me_eventos.h"
#include "Laufey_Nocturne_Interlude_eventos.h"
#include "Laufey_Promise_eventos.h"
#include "Laufey_From_the_Start_eventos.h"
#include "Laufey_misty_eventos.h"
#include "Laufey_Serendipity_eventos.h"
#include "Laufey_Letter_To_My_13_Year_Old_Self_eventos.h"
#include "Laufey_Bewitched_eventos.h"

struct Cancion {
  const char* nombre;
  const char* url;        
  const char* duracion;   
  const Evento* eventos;
  int totalEventos;
};

Cancion canciones[] = {
  { "Dreamer",                       "",  "3:12", Laufey_Dreamer,                      Laufey_Dreamer_total                      },
  { "Second Best",                   "",  "2:22", Laufey_Second_Best,                  Laufey_Second_Best_total                  },
  { "Haunted",                       "",  "1:55", Laufey_Haunted,                      Laufey_Haunted_total                      },
  { "Must Be Love",                  "",  "2:22", Laufey_Must_Be_Love,                 Laufey_Must_Be_Love_total                 },
  { "While You Were Sleeping",       "",  "2:24", Laufey_While_You_Were_Sleeping,      Laufey_While_You_Were_Sleeping_total      },
  { "Lovesick",                      "",  "3:36", Laufey_Lovesick,                     Laufey_Lovesick_total                     },
  { "California and Me",             "",  "2:49", Laufey_California_and_Me,            Laufey_California_and_Me_total            },
  { "Nocturne (Interlude)",          "",  "1:48", Laufey_Nocturne_Interlude,           Laufey_Nocturne_Interlude_total           },
  { "Promise",                       "",  "2:30", Laufey_Promise,                      Laufey_Promise_total                      },
  { "From The Start",                "https://effortless-bienenstitch-a77b48.netlify.app/from_the_start.mp3.mp3",
                                           "2:23", Laufey_From_the_Start,               Laufey_From_the_Start_total               },
  { "Misty",                         "",  "0:50", Laufey_misty,                        Laufey_misty_total                        },
  { "Serendipity",                   "",  "4:57", Laufey_Serendipity,                  Laufey_Serendipity_total                  },
  { "Letter To My 13 Year Old Self", "",  "4:16", Laufey_Letter_To_My_13_Year_Old_Self,Laufey_Letter_To_My_13_Year_Old_Self_total},
  { "Bewitched",                     "",  "5:52", Laufey_Bewitched,                    Laufey_Bewitched_total                    },
};
const int totalCanciones = sizeof(canciones) / sizeof(canciones[0]);

int   cancionActual      = 0;
int   notaActual         = 0;
unsigned long tiempoSiguiente    = 0;
unsigned long tiempoSubirServo[12] = {0};
bool  tocando            = false;
bool  pausado            = false;
unsigned long tiempoPausa = 0;
int   progresoActual     = 0;
String notaActualNombre  = "—";
int   servoActivo[12]    = {0};
int   animacionActual    = 0;   
int frameActual      = 0;
unsigned long ultimoFrame = 0;
const char pagina[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>Caja Musical Laufey</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;
     background:#ABA47E;color:#B5AE84;max-width:420px;margin:0 auto}
.header{background:#615C40;padding:1.2rem 1.25rem;border-bottom:0.5px solid #615C40}
.h-title{font-size:18px;font-weight:500;margin-bottom:3px;color:#C4BE86}
.h-sub{font-size:12px;color:#C4BE86;display:flex;align-items:center;gap:6px}
.dot{width:7px;height:7px;border-radius:50%;background:#E24B4A;display:inline-block}
.section{padding:1rem 1.25rem;border-bottom:0.5px solid #2e2e2b}
.sec-label{font-size:11px;font-weight:500;text-transform:uppercase;
           letter-spacing:.06em;color:#615C40;margin-bottom:.75rem}
.song-item{display:flex;align-items:center;gap:10px;padding:.65rem .75rem;
           border-radius:8px;border:0.5px solid #827F4F;margin-bottom:6px;
           cursor:pointer;background:#615C40;transition:all .15s}
.song-item.active{border-color:#DBA0B2;background:#DBA0B2}
.song-item.active .song-name,.song-item.active .song-dur{color:#FFE0ED}
.song-num{font-size:13px;color:#C4BE86;min-width:16px}
.song-cover{width:36px;height:36px;border-radius:6px;object-fit:cover;
            background:transparent;flex-shrink:0}
.song-info{flex:1}
.song-name{font-size:14px;font-weight:500;color:#C4BE86}
.song-dur{font-size:12px;color:#C4BE86}
.bars{display:flex;gap:2px;align-items:flex-end;height:16px}
.bar{width:3px;border-radius:2px;background:#C4BE86}
.nota-wrap{text-align:center;padding:1rem;background:#C4BE86;
           border-radius:8px;margin-bottom:.75rem}
.nota-big{font-size:32px;font-weight:500;color:#615C40}
.nota-sub{font-size:11px;color:#736E3F;margin-top:3px}
.prog-bar{width:100%;height:4px;background:#515F96;border-radius:20px;
          overflow:hidden;margin-bottom:4px}
.prog-fill{height:100%;background:#7F77DD;border-radius:20px;
           transition:width .3s;width:0%}
.prog-times{display:flex;justify-content:space-between;
            font-size:11px;color:#615C40;margin-bottom:.75rem}
.player{display:flex;align-items:center;justify-content:space-between}
.play-btn{width:56px;height:56px;border-radius:50%;background:transparent;
          display:flex;align-items:center;justify-content:center;
          cursor:pointer;border:none;padding:0}
.play-btn img{width:56px;height:56px;object-fit:contain;border-radius:50%}
.ctrl-btn{width:44px;height:44px;border-radius:50%;border:none;
          display:flex;align-items:center;justify-content:center;
          cursor:pointer;background:transparent;padding:0}
.ctrl-btn img{width:44px;height:44px;object-fit:contain}
.estado-nema{padding:.5rem .75rem;border-radius:8px;background:#B39E40;
             border:0.5px solid #B39E40;font-size:12px;color:#515F96;
             margin-top:.5rem;display:flex;justify-content:space-between}
.servo-grid{display:grid;grid-template-columns:repeat(6,1fr);gap:5px}
.servo{aspect-ratio:1;border-radius:6px;border:0.5px solid #2e2e2b;
       display:flex;align-items:center;justify-content:center;
       font-size:11px;color:#8F96B8;background:#515F96;transition:all .2s}
.servo.on{background:#515F96;color:#5dcaa5;border-color:#5dcaa5;font-weight:500}
.anim-grid{display:grid;grid-template-columns:1fr 1fr 1fr;gap:6px}
.anim-item{border-radius:8px;border:none;cursor:pointer;
           overflow:hidden;padding:0;background:transparent;transition:all .15s}
.anim-item img{width:100%;height:100%;object-fit:cover;display:block;border-radius:8px}
.anim-item.on{outline:3px solid #7F77DD}
.footer{padding:.75rem 1.25rem;display:flex;justify-content:space-between;
        align-items:center;background:#A86D7F}
.footer-txt{font-size:11px;color:#DBA0B2}
.online{font-size:11px;color:#DBA0B2;display:flex;align-items:center;gap:4px}
@keyframes b{0%,100%{transform:scaleY(1)}50%{transform:scaleY(1.6)}}
</style>
</head>
<body>

<div class="header">
  <div class="h-title">&#9834; Caja Musical Laufey</div>
  <div class="h-sub">
    <span class="dot" id="statusDot"></span>
    <span id="statusTxt">Conectando...</span>
  </div>
</div>

<div class="section">
  <div class="sec-label">Bewitched (2023)</div>
  <div id="listaCanciones"></div>
</div>

<div class="section">
  <div class="nota-wrap">
    <div class="nota-big" id="notaActual">&#8212;</div>
    <div class="nota-sub" id="notaSub">esperando...</div>
  </div>
  <div class="prog-bar"><div class="prog-fill" id="progFill"></div></div>
  <div class="prog-times">
    <span id="tiempoActual">0:00</span>
    <span id="tiempoTotal">0:00</span>
  </div>
  <div class="player">
    <div class="ctrl-btn" onclick="anterior()">
      <img src="https://raw.githubusercontent.com/pongofaninata/laufey-/main/atras.png" alt="anterior">
    </div>
    <button class="play-btn" onclick="togglePlay()">
      <img id="imgPlay" src="https://raw.githubusercontent.com/pongofaninata/laufey-/main/starstartt.png" alt="play">
    </button>
    <div class="ctrl-btn" onclick="siguiente()">
      <img src="https://raw.githubusercontent.com/pongofaninata/laufey-/main/sig.png" alt="siguiente">
    </div>
  </div>
  <div class="estado-nema">
    <span id="estadoNema">Eje: en reposo</span>
    <span id="progNema">0%</span>
  </div>
</div>

<div class="section">
  <div class="sec-label">Servos activos</div>
  <div class="servo-grid" id="servoGrid"></div>
</div>

<div class="section">
  <div class="sec-label">Animacion del OLED</div>
  <div class="anim-grid">
    <div class="anim-item on" onclick="setAnimacion(0,this)">
      <img src="https://raw.githubusercontent.com/pongofaninata/laufey-/main/zzz.png" alt="ZZZ">
    </div>
    <div class="anim-item" onclick="setAnimacion(1,this)">
      <img src="https://raw.githubusercontent.com/pongofaninata/laufey-/main/mei.png" alt="Meimei">
    </div>
    <div class="anim-item" onclick="setAnimacion(2,this)">
      <img src="https://raw.githubusercontent.com/pongofaninata/laufey-/main/alas.png" alt="Alas">
    </div>
    <div class="anim-item" onclick="setAnimacion(3,this)">
      <img src="https://raw.githubusercontent.com/pongofaninata/laufey-/main/vinill.png" alt="Vinil">
    </div>
    <div class="anim-item" onclick="setAnimacion(4,this)">
      <img src="https://raw.githubusercontent.com/pongofaninata/laufey-/main/luz.png" alt="Luz">
    </div>
  </div>
</div>

<div class="footer">
  <div class="footer-txt" id="ipTxt">IP: cargando...</div>
  <div class="online"><span class="dot"></span> En linea</div>
</div>

<audio id="audioPlayer" style="display:none"></audio>

<script>
const IMG_PLAY  = "https://raw.githubusercontent.com/pongofaninata/laufey-/main/starstartt.png";
const IMG_PAUSE = "https://raw.githubusercontent.com/pongofaninata/laufey-/main/starpause.png";

// El ESP32 sustituye CANCIONES_JSON por el JSON real en setup()
const canciones = CANCIONES_JSON;

let cancionActual = 0;
let reproduciendo = false;
const audio = document.getElementById('audioPlayer');

function renderCanciones() {
  const lista = document.getElementById('listaCanciones');
  lista.innerHTML = canciones.map((c, i) => `
    <div class="song-item ${i === cancionActual ? 'active' : ''}" onclick="seleccionarCancion(${i})">
      <div class="song-num">${i + 1}</div>
      <div class="song-cover"></div>
      <div class="song-info">
        <div class="song-name">${c.nombre}</div>
        <div class="song-dur">${c.duracion}</div>
      </div>
      ${i === cancionActual && reproduciendo
        ? `<div class="bars">
             <div class="bar" style="height:6px;animation:b 0.8s ease-in-out infinite"></div>
             <div class="bar" style="height:10px;animation:b 0.8s ease-in-out infinite 0.15s"></div>
             <div class="bar" style="height:8px;animation:b 0.8s ease-in-out infinite 0.3s"></div>
           </div>`
        : ''}
    </div>`).join('');
}

function renderServos(activos) {
  const grid = document.getElementById('servoGrid');
  grid.innerHTML = Array.from({ length: 12 }, (_, i) =>
    `<div class="servo ${activos[i] ? 'on' : ''}">${i + 1}</div>`
  ).join('');
}

function seleccionarCancion(i) {
  if (reproduciendo) {
    fetch('/cambiar?i=' + i).then(() => {
      cancionActual = i;
      reproduciendo = false;
      document.getElementById('imgPlay').src = IMG_PLAY;
      audio.pause();
      renderCanciones();
    });
  } else {
    cancionActual = i;
    fetch('/seleccionar?i=' + i);
    renderCanciones();
  }
}

function togglePlay() {
  if (!reproduciendo) iniciar();
  else pausar();
}

function iniciar() {
  if (canciones[cancionActual].url) {
    audio.src = canciones[cancionActual].url;
    audio.currentTime = 0;
    audio.play().then(() => { fetch('/play?i=' + cancionActual); });
  } else {
    fetch('/play?i=' + cancionActual);
  }
  reproduciendo = true;
  document.getElementById('imgPlay').src = IMG_PAUSE;
  renderCanciones();
}

function pausar() {
  if (!audio.paused) {
    audio.pause();
    fetch('/pause');
    document.getElementById('imgPlay').src = IMG_PLAY;
  } else {
    audio.play();
    fetch('/resume');
    document.getElementById('imgPlay').src = IMG_PAUSE;
  }
  reproduciendo = !audio.paused;
}

function anterior() {
  seleccionarCancion((cancionActual - 1 + canciones.length) % canciones.length);
}

function siguiente() {
  seleccionarCancion((cancionActual + 1) % canciones.length);
}

function setAnimacion(a, el) {
  document.querySelectorAll('.anim-item').forEach(e => e.classList.remove('on'));
  el.classList.add('on');
  fetch('/animacion?a=' + a);
}

function actualizarEstado() {
  fetch('/estado')
    .then(r => r.json())
    .then(d => {
      document.getElementById('notaActual').textContent = d.nota || '\u2014';
      document.getElementById('notaSub').textContent = d.freq ? d.freq + ' Hz' : 'esperando...';
      document.getElementById('progFill').style.width = d.progreso + '%';
      document.getElementById('estadoNema').textContent =
        d.regresando ? '< Regresando...' : d.tocando ? '> Avanzando' : 'Eje: en reposo';
      document.getElementById('progNema').textContent = Math.round(d.progNema) + '%';
      renderServos(d.servos || new Array(12).fill(0));
      document.getElementById('statusDot').style.background = '#5dcaa5';
      document.getElementById('statusTxt').textContent = 'Conectada';
      document.getElementById('ipTxt').textContent = 'IP: ' + d.ip;
      if (d.fin && reproduciendo) {
        reproduciendo = false;
        document.getElementById('imgPlay').src = IMG_PLAY;
        siguiente();
      }
    })
    .catch(() => {
      document.getElementById('statusDot').style.background = '#E24B4A';
      document.getElementById('statusTxt').textContent = 'Sin conexion';
    });
}

audio.addEventListener('timeupdate', () => {
  const t = audio.currentTime, dur = audio.duration || 0;
  document.getElementById('tiempoActual').textContent =
    Math.floor(t / 60) + ':' + String(Math.floor(t % 60)).padStart(2, '0');
  document.getElementById('tiempoTotal').textContent =
    Math.floor(dur / 60) + ':' + String(Math.floor(dur % 60)).padStart(2, '0');
});

audio.addEventListener('ended', () => {
  fetch('/stop');
  reproduciendo = false;
  document.getElementById('imgPlay').src = IMG_PLAY;
  siguiente();
});

renderCanciones();
renderServos(new Array(12).fill(0));
setInterval(actualizarEstado, 400);
</script>
</body>
</html>
)rawliteral";
//hardware
void moverServo(int canal, int angulo) {
  int pulso = map(angulo, 0, 180, SERVO_MIN, SERVO_MAX);
  pca.setPWM(canal, 0, pulso);
}

void subirTodos() {
  for (int i = 0; i < 12; i++) {
    moverServo(i, 90);
    servoActivo[i] = 0;
  }
}
void animarOLED() {
  if (animacionActual == 0) {
    laufeyUpdate();
  } else {
    if (millis() - ultimoFrame > 200) {
      ultimoFrame = millis();
      display.clearDisplay();
      display.setTextColor(SSD1306_WHITE);
      display.setTextSize(3);
      display.setCursor(10, 5);
      display.print(notaActualNombre);
      display.setTextSize(1);
      display.setCursor(0, 44);
      display.print(canciones[cancionActual].nombre);
      display.drawRect(0, 56, 128, 7, SSD1306_WHITE);
      display.fillRect(1, 57, map(progresoActual, 0, 100, 0, 126), 5, SSD1306_WHITE);
      display.display();
    }
  }
}
void iniciarCancion(int indice) {
  cancionActual    = indice;
  notaActual       = 0;
  tiempoSiguiente  = millis();
  for (int i = 0; i < 12; i++) tiempoSubirServo[i] = 0;
  progresoActual   = 0;
  notaActualNombre = "—";
  subirTodos();
  tocando = true;
  pausado = false;
  laufeySong(canciones[indice].nombre);
}

//setup
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pca.begin();
  pca.setPWMFreq(50);
  delay(10);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(5, 10);
  display.print("LAUFEY");
  display.drawLine(0, 30, 128, 30, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(5, 38);
  display.print("Conectando WiFi...");
  display.display();
  laufeyBegin(display);
  subirTodos();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  String ip = WiFi.localIP().toString();
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);  display.print("WiFi OK!");
  display.setCursor(0, 14); display.print(ip);
  display.setCursor(0, 28); display.print("Abre en celular:");
  display.setCursor(0, 40); display.print("http://" + ip);
  display.display();
  Serial.println("IP: " + ip);
  String cancionesJson = "[";
  for (int i = 0; i < totalCanciones; i++) {
    if (i > 0) cancionesJson += ",";
    cancionesJson += "{\"nombre\":\"" + String(canciones[i].nombre) + "\",";
    cancionesJson += "\"url\":\""     + String(canciones[i].url)     + "\",";
    cancionesJson += "\"duracion\":\"" + String(canciones[i].duracion) + "\"}";
  }
  cancionesJson += "]";

  String paginaFinal = String(pagina);
  paginaFinal.replace("CANCIONES_JSON", cancionesJson);
  server.on("/", [paginaFinal]() {
    server.send(200, "text/html", paginaFinal);
  });

  server.on("/play", []() {
    int idx = server.arg("i").toInt();
    if (idx >= 0 && idx < totalCanciones) iniciarCancion(idx);
    server.send(200, "text/plain", "ok");
  });

  server.on("/seleccionar", []() {
    int idx = server.arg("i").toInt();
    if (idx >= 0 && idx < totalCanciones) cancionActual = idx;
    server.send(200, "text/plain", "ok");
  });

  server.on("/cambiar", []() {
    int idx = server.arg("i").toInt();
    if (idx >= 0 && idx < totalCanciones) {
      tocando = false;
      pausado = false;
      subirTodos();
      cancionActual    = idx;
      notaActualNombre = "—";
      progresoActual   = 0;
    }
    server.send(200, "text/plain", "ok");
  });

  server.on("/pause", []() {
    if (!pausado) {
      pausado = true;
      tiempoPausa = millis();
    }
    server.send(200, "text/plain", "ok");
  });

  server.on("/resume", []() {
    if (pausado) {
      pausado = false;
      tiempoSiguiente += millis() - tiempoPausa;
    }
    server.send(200, "text/plain", "ok");
  });

  server.on("/stop", []() {
    tocando          = false;
    pausado          = false;
    notaActual       = 0;
    notaActualNombre = "—";
    progresoActual   = 0;
    subirTodos();
    server.send(200, "text/plain", "ok");
  });

  server.on("/animacion", []() {
    int a = server.arg("a").toInt();
    if (a >= 0 && a <= 4) animacionActual = a;
    server.send(200, "text/plain", "ok");
  });
  server.on("/estado", []() {
    String servosJson = "[";
    for (int i = 0; i < 12; i++) {
      if (i > 0) servosJson += ",";
      servosJson += String(servoActivo[i]);
    }
    servosJson += "]";
    int freqActual = 0;
    if (notaActualNombre != "—" && notaActual > 0 && notaActual <= canciones[cancionActual].totalEventos) {
      int teclaIdx = canciones[cancionActual].eventos[notaActual - 1].tecla;
      if (teclaIdx >= 0 && teclaIdx < 12) freqActual = TECLAS[teclaIdx];
    }

    bool fin = (!tocando && progresoActual == 100);

    String json = "{";
    json += "\"nota\":\""   + notaActualNombre + "\",";
    json += "\"freq\":"     + String(freqActual) + ",";
    json += "\"progreso\":" + String(progresoActual) + ",";
    json += "\"tocando\":"  + String(tocando  ? "true" : "false") + ",";
    json += "\"pausado\":"  + String(pausado  ? "true" : "false") + ",";
    json += "\"fin\":"      + String(fin       ? "true" : "false") + ",";
    json += "\"regresando\":false,";   // reservado para NEMA futuro
    json += "\"progNema\":0,";         // reservado para NEMA futuro
    json += "\"servos\":"   + servosJson + ",";
    json += "\"ip\":\""     + WiFi.localIP().toString() + "\"";
    json += "}";
    server.send(200, "application/json", json);
  });

  server.begin();
  iniciarCancion(0);
}
void loop() {
  server.handleClient();
  animarOLED();

  if (!tocando || pausado) return;

  unsigned long ahora = millis();

  for (int i = 0; i < 12; i++) {
    if (tiempoSubirServo[i] > 0 && ahora >= tiempoSubirServo[i]) {
      moverServo(i, 90);
      servoActivo[i]    = 0;
      tiempoSubirServo[i] = 0;
    }
  }

  Cancion& c = canciones[cancionActual];
  if (notaActual < c.totalEventos && ahora >= tiempoSiguiente) {
    Evento ev;
    memcpy(&ev, &c.eventos[notaActual], sizeof(Evento));

    int tecla = ev.tecla;
    int dur   = ev.duracion;

    if (tecla >= 0 && tecla < 12) {
      moverServo(tecla, 30);
      tiempoSubirServo[tecla] = ahora + dur - 50;
      servoActivo[tecla]      = 1;
      notaActualNombre        = NOMBRES_TECLAS[tecla];
      progresoActual          = map(notaActual, 0, c.totalEventos, 0, 100);
      Serial.println(notaActualNombre);
    } else {
      notaActualNombre = "—";
    }

    tiempoSiguiente = ahora + dur;
    notaActual++;
  }
  if (notaActual >= c.totalEventos) {
    tocando          = false;
    subirTodos();
    notaActualNombre = "Fin";
    progresoActual   = 100;
  }
}
