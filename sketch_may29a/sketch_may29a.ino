#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_PWMServoDriver.h>
#include "frames_laufey.h"

// ── Primero el struct Evento ──
struct Evento { int tecla; int duracion; };

// ── Luego todos los archivos de eventos ──
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

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);
WebServer server(80);

const char* ssid     = "INFINITUM52CA_2.4";
const char* password = "PnQU6XeT7P";

#define STEP_PIN    18
#define DIR_PIN     19
#define SERVO_MIN   150
#define SERVO_MAX   600
#define PASOS_TOTAL 120000L

#define BASE_URL "https://effortless-bienenstitch-a77b48.netlify.app/"

int TECLAS[12] = {554,622,698,740,831,932,1047,1109,1245,1397,1480,1661};
String NOMBRES_TECLAS[12] = {
  "Db5","Eb5","F5","Gb5","Ab5","Bb5",
  "C6","Db6","Eb6","F6","Gb6","Ab6"
};

struct Cancion {
  const char* nombre;
  const char* url;
  const Evento* eventos;
  int totalEventos;
  long duracionMs;
  const char* duracion;
};

Cancion canciones[] = {
  {"Dreamer", BASE_URL "laufey%20-%20dreamer%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_Dreamer, Laufey_Dreamer_total, Laufey_Dreamer_duracion, "3:12"},
  {"Second Best", BASE_URL "laufey%20-%20second%20best%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_Second_Best, Laufey_Second_Best_total, Laufey_Second_Best_duracion, "2:22"},
  {"Haunted", BASE_URL "laufey%20-%20haunted%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_Haunted, Laufey_Haunted_total, Laufey_Haunted_duracion, "1:55"},
  {"Must Be Love", BASE_URL "laufey%20-%20must%20be%20love%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_Must_Be_Love, Laufey_Must_Be_Love_total, Laufey_Must_Be_Love_duracion, "2:22"},
  {"While You Were Sleeping", BASE_URL "laufey%20-%20while%20you%20were%20sleeping%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_While_You_Were_Sleeping, Laufey_While_You_Were_Sleeping_total, Laufey_While_You_Were_Sleeping_duracion, "2:24"},
  {"Lovesick", BASE_URL "laufey%20-%20lovesick%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_Lovesick, Laufey_Lovesick_total, Laufey_Lovesick_duracion, "3:36"},
  {"California and Me", BASE_URL "laufey%20-%20california%20and%20me%20feat.%20philharmonia%20orchestra%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_California_and_Me, Laufey_California_and_Me_total, Laufey_California_and_Me_duracion, "2:49"},
  {"Nocturne Interlude", BASE_URL "laufey%20-%20nocturne%20interlude%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_Nocturne_Interlude, Laufey_Nocturne_Interlude_total, Laufey_Nocturne_Interlude_duracion, "1:48"},
  {"Promise", BASE_URL "laufey%20-%20promise%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_Promise, Laufey_Promise_total, Laufey_Promise_duracion, "2:30"},
  {"From the Start", BASE_URL "laufey%20-%20from%20the%20start%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_From_the_Start, Laufey_From_the_Start_total, Laufey_From_the_Start_duracion, "2:23"},
  {"Misty", BASE_URL "laufey%20-%20misty%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_misty, Laufey_misty_total, Laufey_misty_duracion, "0:50"},
  {"Serendipity", BASE_URL "laufey%20-%20serendipity%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_Serendipity, Laufey_Serendipity_total, Laufey_Serendipity_duracion, "4:57"},
  {"Letter to My 13 Year Old Self", BASE_URL "laufey%20-%20letter%20to%20my%2013%20year%20old%20self%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_Letter_To_My_13_Year_Old_Self, Laufey_Letter_To_My_13_Year_Old_Self_total, Laufey_Letter_To_My_13_Year_Old_Self_duracion, "4:16"},
  {"Bewitched", BASE_URL "laufey%20-%20bewitched%20(official%20lyric%20video%20with%20chords).mp3",
   Laufey_Bewitched, Laufey_Bewitched_total, Laufey_Bewitched_duracion, "5:52"},
};
int totalCanciones = 14;
int cancionActual  = 0;

int frameActual = 0;
unsigned long ultimoFrame = 0;
int notaActual = 0;
unsigned long tiempoSiguiente  = 0;
unsigned long tiempoSubirServo[12] = {0};
bool tocando   = false;
bool pausado   = false;
unsigned long tiempoInicioPausa = 0;
int progresoActual = 0;
String notaActualNombre = "";
int servoActivo[12] = {0};
int animacionActual = 0;  // 0=Laufey duerme, 1=Conejo, 2=Laufey chiquita
bool audioListo = false;

long pasosDados  = 0;
float pasoAcum   = 0;
float pasosPorMs = 0;
unsigned long ultimoPaso = 0;
bool regresando  = false;

void moverServo(int canal, int angulo) {
  if (canal >= 0 && canal < 12)
    pca.setPWM(canal, 0, map(angulo, 0, 180, SERVO_MIN, SERVO_MAX));
}

void subirTodos() {
  for (int i = 0; i < 12; i++) {
    moverServo(i, 90);
    servoActivo[i] = 0;
    tiempoSubirServo[i] = 0;
  }
}

void regresarNEMA() {
  regresando = true;
  digitalWrite(DIR_PIN, LOW);
}

void animarOLED() {
  if (millis() - ultimoFrame > 200) {
    ultimoFrame = millis();
    display.clearDisplay();

    // Calcular frame según animación seleccionada
    // Grupo 0: frames 0-2, Grupo 1: frames 3-5, Grupo 2: frames 6-8
    int frameOffset = animacionActual * 3;
    int frameLocal  = frameOffset + (frameActual % 3);

    display.drawBitmap(0, 0, animacion[frameLocal], 128, 64, WHITE);
    display.setTextColor(BLACK);
    display.setTextSize(1);
    display.setCursor(2, 2);
    if (regresando)        display.print("< Regresando...");
    else if (pausado)      display.print("II Pausado");
    else if (!audioListo)  display.print("Listo - da Play");
    else                   display.print(canciones[cancionActual].nombre);
    display.setCursor(2, 54);
    display.print(notaActualNombre);
    display.fillRect(0, 62, map(progresoActual, 0, 100, 0, 128), 2, BLACK);

    display.display();
    frameActual = (frameActual + 1) % 3;
  }
}

void iniciarCancion(int indice) {
  cancionActual    = indice;
  notaActual       = 0;
  tiempoSiguiente  = millis() + 300;
  for (int i = 0; i < 12; i++) tiempoSubirServo[i] = 0;
  progresoActual   = 0;
  notaActualNombre = "";
  subirTodos();
  tocando    = true;
  pausado    = false;
  regresando = false;
  audioListo = true;
  pasosDados = 0;
  pasoAcum   = 0;
  ultimoPaso = millis();
  pasosPorMs = (float)PASOS_TOTAL / canciones[indice].duracionMs;
  digitalWrite(DIR_PIN, HIGH);
  Serial.print("Cancion: ");
  Serial.println(canciones[indice].nombre);
}

void detenerTodo() {
  tocando    = false;
  pausado    = false;
  audioListo = false;
  notaActual = 0;
  subirTodos();
  notaActualNombre = "";
  progresoActual   = 0;
}

const char pagina[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width,initial-scale=1'>
<title>Caja Musical Laufey</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;
     background:#0f0f0d;color:#e8e6de;max-width:420px;margin:0 auto}
.header{background:#1a1a17;padding:1.2rem 1.25rem;border-bottom:0.5px solid #2e2e2b}
.h-title{font-size:18px;font-weight:500;margin-bottom:3px}
.h-sub{font-size:12px;color:#9a9890;display:flex;align-items:center;gap:6px}
.dot{width:7px;height:7px;border-radius:50%;background:#5dcaa5;display:inline-block}
.section{padding:1rem 1.25rem;border-bottom:0.5px solid #2e2e2b}
.sec-label{font-size:11px;font-weight:500;text-transform:uppercase;
           letter-spacing:.06em;color:#6a6a62;margin-bottom:.75rem}
.song-item{display:flex;align-items:center;gap:10px;padding:.65rem .75rem;
           border-radius:8px;border:0.5px solid #2e2e2b;margin-bottom:6px;
           cursor:pointer;background:#1a1a17;transition:all .15s}
.song-item.active{border-color:#5dcaa5;background:#04342C}
.song-item.active .song-name,.song-item.active .song-dur{color:#5dcaa5}
.song-num{font-size:13px;color:#6a6a62;min-width:16px}
.song-info{flex:1}
.song-name{font-size:14px;font-weight:500;color:#e8e6de}
.song-dur{font-size:12px;color:#6a6a62}
.nota-wrap{text-align:center;padding:1rem;background:#1a1a17;
           border-radius:8px;margin-bottom:.75rem}
.nota-big{font-size:32px;font-weight:500}
.nota-sub{font-size:11px;color:#6a6a62;margin-top:3px}
.prog-bar{width:100%;height:4px;background:#2e2e2b;border-radius:20px;
          overflow:hidden;margin-bottom:4px}
.prog-fill{height:100%;background:#5dcaa5;border-radius:20px;
           transition:width .3s;width:0%}
.prog-times{display:flex;justify-content:space-between;
            font-size:11px;color:#6a6a62;margin-bottom:.75rem}
.player{display:flex;align-items:center;justify-content:space-between}
.play-btn{width:48px;height:48px;border-radius:50%;background:#5dcaa5;
          display:flex;align-items:center;justify-content:center;
          font-size:20px;cursor:pointer;border:none;color:#04342C}
.ctrl-btn{width:38px;height:38px;border-radius:50%;
          border:0.5px solid #3a3a38;display:flex;align-items:center;
          justify-content:center;color:#9a9890;font-size:16px;
          cursor:pointer;background:transparent}
.estado-nema{padding:.5rem .75rem;border-radius:8px;background:#1a1a17;
             border:0.5px solid #2e2e2b;font-size:12px;color:#6a6a62;
             margin-top:.5rem;display:flex;justify-content:space-between}
.servo-grid{display:grid;grid-template-columns:repeat(6,1fr);gap:5px}
.servo{aspect-ratio:1;border-radius:6px;border:0.5px solid #2e2e2b;
       display:flex;align-items:center;justify-content:center;
       font-size:11px;color:#6a6a62;background:#1a1a17;transition:all .2s}
.servo.on{background:#0d2e26;color:#5dcaa5;border-color:#5dcaa5;font-weight:500}
.anim-grid{display:grid;grid-template-columns:1fr 1fr 1fr;gap:6px}
.anim-item{padding:.65rem .75rem;border-radius:8px;
           border:0.5px solid #2e2e2b;cursor:pointer;
           background:#1a1a17;transition:all .15s}
.anim-item.on{border-color:#7F77DD;background:#26215C}
.anim-item.on .anim-name{color:#CECBF6}
.anim-name{font-size:12px;font-weight:500;color:#e8e6de;margin-bottom:2px}
.anim-sub{font-size:10px;color:#6a6a62}
.footer{padding:.75rem 1.25rem;display:flex;justify-content:space-between;
        align-items:center;background:#1a1a17}
.footer-txt{font-size:11px;color:#6a6a62}
.online{font-size:11px;color:#5dcaa5;display:flex;align-items:center;gap:4px}
</style>
</head>
<body>
<div class="header">
  <div class="h-title">Caja Musical Laufey</div>
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
    <div class="nota-big" id="notaActual">-</div>
    <div class="nota-sub" id="notaSub">esperando...</div>
  </div>
  <div class="prog-bar"><div class="prog-fill" id="progFill"></div></div>
  <div class="prog-times">
    <span id="tiempoActual">0:00</span>
    <span id="tiempoTotal">0:00</span>
  </div>
  <div class="player">
    <div class="ctrl-btn" onclick="anterior()">&#9198;</div>
    <div class="play-btn" id="playBtn" onclick="togglePlay()">&#9654;</div>
    <div class="ctrl-btn" onclick="siguiente()">&#9197;</div>
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
      <div class="anim-name">Laufey duerme</div>
      <div class="anim-sub">ZZZ</div>
    </div>
    <div class="anim-item" onclick="setAnimacion(1,this)">
      <div class="anim-name">Conejo</div>
      <div class="anim-sub">Meimei</div>
    </div>
    <div class="anim-item" onclick="setAnimacion(2,this)">
      <div class="anim-name">Laufey chiquita</div>
      <div class="anim-sub">Con alas</div>
    </div>
  </div>
</div>
<div class="footer">
  <div class="footer-txt" id="ipTxt">IP: cargando...</div>
  <div class="online"><span class="dot"></span> En linea</div>
</div>
<audio id="audioPlayer" style="display:none"></audio>
<script>
const canciones = CANCIONES_JSON;
let cancionActual = 0;
let reproduciendo = false;
const audio = document.getElementById('audioPlayer');
function renderCanciones() {
  const lista = document.getElementById('listaCanciones');
  lista.innerHTML = canciones.map((c,i) => `
    <div class="song-item ${i===cancionActual?'active':''}" onclick="seleccionarCancion(${i})">
      <div class="song-num">${i+1}</div>
      <div class="song-info">
        <div class="song-name">${c.nombre}</div>
        <div class="song-dur">${c.duracion}</div>
      </div>
    </div>`).join('');
}
function renderServos(activos) {
  const grid = document.getElementById('servoGrid');
  grid.innerHTML = Array.from({length:12},(_,i) =>
    `<div class="servo ${activos[i]?'on':''}">${i+1}</div>`).join('');
}
function seleccionarCancion(i) {
  if (reproduciendo) {
    fetch('/cambiar?i='+i).then(()=>{
      cancionActual = i;
      reproduciendo = false;
      document.getElementById('playBtn').innerHTML = '&#9654;';
      audio.pause();
      renderCanciones();
    });
  } else {
    cancionActual = i;
    fetch('/seleccionar?i='+i);
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
    audio.play().then(() => { fetch('/play?i='+cancionActual); });
  } else {
    fetch('/play?i='+cancionActual);
  }
  reproduciendo = true;
  document.getElementById('playBtn').innerHTML = '&#9646;&#9646;';
  renderCanciones();
}
function pausar() {
  if (!audio.paused) {
    audio.pause();
    fetch('/pause');
    document.getElementById('playBtn').innerHTML = '&#9654;';
  } else {
    audio.play();
    fetch('/resume');
    document.getElementById('playBtn').innerHTML = '&#9646;&#9646;';
  }
  reproduciendo = !audio.paused;
}
function anterior() {
  seleccionarCancion((cancionActual-1+canciones.length)%canciones.length);
}
function siguiente() {
  seleccionarCancion((cancionActual+1)%canciones.length);
}
function setAnimacion(a,el) {
  document.querySelectorAll('.anim-item').forEach(e=>e.classList.remove('on'));
  el.classList.add('on');
  fetch('/animacion?a='+a);
}
function actualizarEstado() {
  fetch('/estado')
    .then(r=>r.json())
    .then(d=>{
      document.getElementById('notaActual').textContent = d.nota||'-';
      document.getElementById('notaSub').textContent = d.freq ? d.freq+' Hz' : 'esperando...';
      document.getElementById('progFill').style.width = d.progreso+'%';
      document.getElementById('estadoNema').textContent =
        d.regresando ? '< Regresando...' : d.tocando ? '> Avanzando' : 'Eje: en reposo';
      document.getElementById('progNema').textContent = Math.round(d.progNema)+'%';
      renderServos(d.servos||new Array(12).fill(0));
      document.getElementById('statusDot').style.background='#5dcaa5';
      document.getElementById('statusTxt').textContent='Conectada';
      document.getElementById('ipTxt').textContent='IP: '+d.ip;
      if (d.fin && reproduciendo) {
        reproduciendo = false;
        document.getElementById('playBtn').innerHTML='&#9654;';
      }
    }).catch(()=>{
      document.getElementById('statusDot').style.background='#E24B4A';
      document.getElementById('statusTxt').textContent='Sin conexion';
    });
}
audio.addEventListener('timeupdate',()=>{
  const t=audio.currentTime, dur=audio.duration||0;
  document.getElementById('tiempoActual').textContent=
    Math.floor(t/60)+':'+String(Math.floor(t%60)).padStart(2,'0');
  document.getElementById('tiempoTotal').textContent=
    Math.floor(dur/60)+':'+String(Math.floor(dur%60)).padStart(2,'0');
});
audio.addEventListener('ended',()=>{
  fetch('/stop');
  reproduciendo=false;
  document.getElementById('playBtn').innerHTML='&#9654;';
});
renderCanciones();
renderServos(new Array(12).fill(0));
setInterval(actualizarEstado,400);
</script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  pca.begin();
  pca.setPWMFreq(50);
  delay(10);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, HIGH);

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

  subirTodos();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  String ip = WiFi.localIP().toString();
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("WiFi OK!");
  display.setCursor(0, 14);
  display.print(ip);
  display.setCursor(0, 28);
  display.print("Abre en celular:");
  display.setCursor(0, 40);
  display.print("http://" + ip);
  display.display();
  Serial.println("IP: " + ip);
  delay(5000);

  String cancionesJson = "[";
  for (int i = 0; i < totalCanciones; i++) {
    if (i > 0) cancionesJson += ",";
    cancionesJson += "{\"nombre\":\"" + String(canciones[i].nombre) + "\",";
    cancionesJson += "\"url\":\"" + String(canciones[i].url) + "\",";
    cancionesJson += "\"duracion\":\"" + String(canciones[i].duracion) + "\"}";
  }
  cancionesJson += "]";

  String paginaFinal = String(pagina);
  paginaFinal.replace("CANCIONES_JSON", cancionesJson);

  server.on("/", [paginaFinal]() {
    server.send(200, "text/html", paginaFinal);
  });
  server.on("/play", []() {
    iniciarCancion(server.arg("i").toInt());
    server.send(200, "text/plain", "ok");
  });
  server.on("/seleccionar", []() {
    if (!tocando) cancionActual = server.arg("i").toInt();
    server.send(200, "text/plain", "ok");
  });
  server.on("/cambiar", []() {
    detenerTodo();
    cancionActual = server.arg("i").toInt();
    regresarNEMA();
    server.send(200, "text/plain", "ok");
  });
  server.on("/pause", []() {
    pausado = true;
    tiempoInicioPausa = millis();
    server.send(200, "text/plain", "ok");
  });
  server.on("/resume", []() {
    if (pausado) {
      unsigned long durPausa = millis() - tiempoInicioPausa;
      tiempoSiguiente += durPausa;
      ultimoPaso += durPausa;
      pausado = false;
    }
    server.send(200, "text/plain", "ok");
  });
  server.on("/stop", []() {
    detenerTodo();
    regresarNEMA();
    server.send(200, "text/plain", "ok");
  });
  server.on("/animacion", []() {
    animacionActual = server.arg("a").toInt();
    frameActual = 0;
    server.send(200, "text/plain", "ok");
  });
  server.on("/estado", []() {
    String servosJson = "[";
    for (int i = 0; i < 12; i++) {
      if (i > 0) servosJson += ",";
      servosJson += String(servoActivo[i]);
    }
    servosJson += "]";
    int progNema = (int)((float)pasosDados / PASOS_TOTAL * 100);
    String json = "{";
    json += "\"nota\":\"" + notaActualNombre + "\",";
    json += "\"freq\":" + String(
      notaActual < canciones[cancionActual].totalEventos &&
      canciones[cancionActual].eventos[notaActual].tecla >= 0 ?
      TECLAS[canciones[cancionActual].eventos[notaActual].tecla] : 0) + ",";
    json += "\"progreso\":" + String(progresoActual) + ",";
    json += "\"progNema\":" + String(progNema) + ",";
    json += "\"tocando\":" + String(tocando ? "true" : "false") + ",";
    json += "\"regresando\":" + String(regresando ? "true" : "false") + ",";
    json += "\"fin\":" + String(!tocando && !regresando && progresoActual >= 100 ? "true" : "false") + ",";
    json += "\"servos\":" + servosJson + ",";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\"";
    json += "}";
    server.send(200, "application/json", json);
  });
  server.begin();
}

void loop() {
  server.handleClient();
  animarOLED();

  unsigned long ahora = millis();

  // NEMA regresando
  if (regresando) {
    if (pasosDados > 0) {
      for (int i = 0; i < 50 && pasosDados > 0; i++) {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(200);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(200);
        pasosDados--;
      }
    } else {
      regresando = false;
      digitalWrite(DIR_PIN, HIGH);
      Serial.println("NEMA en posicion inicial");
    }
    return;
  }

  // NEMA avanzando
  if (tocando && !pausado && pasosDados < PASOS_TOTAL) {
    unsigned long elapsed = ahora - ultimoPaso;
    pasoAcum += pasosPorMs * elapsed;
    ultimoPaso = ahora;
    while (pasoAcum >= 1.0 && pasosDados < PASOS_TOTAL) {
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(2);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(2);
      pasosDados++;
      pasoAcum -= 1.0;
    }
  }

  if (!tocando || pausado) return;

  // Subir servos
  for (int i = 0; i < 12; i++) {
    if (tiempoSubirServo[i] > 0 && ahora >= tiempoSubirServo[i]) {
      moverServo(i, 90);
      servoActivo[i] = 0;
      tiempoSubirServo[i] = 0;
    }
  }

  // Tocar evento
  Cancion& c = canciones[cancionActual];
  if (notaActual < c.totalEventos && ahora >= tiempoSiguiente) {
    int tecla = c.eventos[notaActual].tecla;
    int dur   = c.eventos[notaActual].duracion;

    if (tecla >= 0 && tecla < 12) {
      moverServo(tecla, 30);
      tiempoSubirServo[tecla] = ahora + dur - 30;
      servoActivo[tecla] = 1;
      notaActualNombre = NOMBRES_TECLAS[tecla];
      progresoActual = map(notaActual, 0, c.totalEventos, 0, 100);
      Serial.println(notaActualNombre);
    } else {
      notaActualNombre = "";
    }

    tiempoSiguiente = ahora + dur;
    notaActual++;
  }

  // Fin de cancion
  if (notaActual >= c.totalEventos) {
    tocando = false;
    subirTodos();
    notaActualNombre = "Fin";
    progresoActual = 100;
    Serial.println("Fin - regresando NEMA");
    regresarNEMA();
  }
}
