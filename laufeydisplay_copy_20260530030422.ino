#include <WiFi.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include <TJpgDec.h>      

TFT_eSPI tft = TFT_eSPI();

const char* SSID     = "TU_WIFI";
const char* PASSWORD = "TU_PASSWORD";



struct Animation {
  const char* name;
  const char* frames[10];   
  int frameCount;
  int frameDelay;          
};

Animation animations[] = {

  {
    "laufeydurmiendo",
    {
 //url
    },
    3,    // num frames
    400   // ms entre frame entre mas va mas lento
  },

  {
    "laufeychiquita",
    {
      //url
    },
    3,
    300
  },

  {
    "meimei",
    {
     //url
    },
    4,
    200  

};

struct SongMap {
  const char* songName;
  int animIndex;
};

SongMap songList[] = {
  { "From the Start",          0 },  // sleep
  { "Bewitched",               0 },  // sleep
  { "Let You Break My Heart",  1 },  // chiquita
  { "Valentine",               1 },  // chiquita
  { "Magnolia",                2 },  // meimei
  { "A Night to Remember",     2 },  // meimei
};

//estado
int currentAnim    = 0;
int currentFrame   = 0;
unsigned long lastFrameTime = 0;

// bufferparaimagenHTTP
uint8_t imgBuffer[30000];  //ram
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void drawFrameFromURL(const char* url) {
  HTTPClient http;
  http.begin(url);
  http.setTimeout(5000);

  int code = http.GET();
  if (code != 200) {
    Serial.printf("Error HTTP %d en: %s\n", code, url);
    http.end();
    return;
  }

  int len = http.getSize();
  if (len <= 0 || len > sizeof(imgBuffer)) {
    Serial.println("Imagen demasiado grande o tamano desconocido");
    http.end();
    return;
  }

  WiFiClient* stream = http.getStreamPtr();
  int bytesRead = 0;
  while (http.connected() && bytesRead < len) {
    int available = stream->available();
    if (available) {
      bytesRead += stream->readBytes(imgBuffer + bytesRead, available);
    }
    delay(1);
  }
  http.end();


  TJpgDec.setJpgScale(1);                    // 1 = tamanio completo
  TJpgDec.setCallback(tft_output);
  TJpgDec.drawJpg(0, 0, imgBuffer, bytesRead);

  Serial.printf("Frame dibujado: %s\n", url);
}
//animacion x cancion
void setSongAnimation(const char* song) {
  int numSongs = sizeof(songList) / sizeof(songList[0]);
  for (int i = 0; i < numSongs; i++) {
    if (strstr(song, songList[i].songName) != NULL) {
      if (currentAnim != songList[i].animIndex) {
        currentAnim  = songList[i].animIndex;
        currentFrame = 0;
        Serial.printf("Animacion cambiada a: %s\n", animations[currentAnim].name);
      }
      return;
    }
  }
}

}
//setup
void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);   // pantalla 1-3
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 10);
  tft.print("Conectando WiFi...");

  WiFi.begin(SSID, PASSWORD);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi OK: " + WiFi.localIP().toString());
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("Laufey Music Box");
  } else {
    tft.setCursor(10, 10);
    tft.print("Sin WiFi :(");
  }

  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);
}
 
//lopdeframes
}

void loop() {
  //cancion
  // serial, cambio de cancion
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    setSongAnimation(cmd.c_str());
  }

  // avanzaseguneltiempo
  unsigned long now = millis();
  Animation& anim = animations[currentAnim];

  if (now - lastFrameTime >= (unsigned long)anim.frameDelay) {
    lastFrameTime = now;

    const char* url = anim.frames[currentFrame];
    drawFrameFromURL(url);

    currentFrame = (currentFrame + 1) % anim.frameCount; // loop
  }
}

}
