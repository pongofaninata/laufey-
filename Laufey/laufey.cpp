#include "laufey.h"
#include "bitmapssss.h"

static Adafruit_SSD1306* _display = nullptr;

struct Animation {
  const char* name;
  const uint8_t* frames[3];
  int frameCount;
  int frameDelay;
};

// 5 animaciones: 0=sleep, 1=chiquita, 2=meimei, 3=vinil, 4=faro
static Animation animations[] = {
  { "laufeydurmiendo", { laufeysleepsi,   laufeysleepsi2, laufeysleepsi3 }, 3, 400 },
  { "laufeychiquita",  { laufeychiquita1, laufeychic2,    launfeychic3   }, 3, 300 },
  { "meimei",          { meimei1,         meimei2,        meimei3        }, 3, 200 },
  { "vinil",           { vinil1,          vinil2,         vinil3         }, 3, 250 },
  { "faro",            { faro1,           faro2,          faro3          }, 3, 300 },
};

// Qué animación va con cada canción
struct SongMap { const char* songName; int animIndex; };

static SongMap songList[] = {
  { "Dreamer",                      2 },  
  { "Second Best",                  0 },  
  { "Haunted",                      1 },  
  { "Must Be Love",                 4 },  
  { "While You Were Sleeping",      0 },  
  { "Lovesick",                     3 },  
  { "California and Me",             2 },  
  { "Nocturne",                      4 },  
  { "Promise",                       2 },  
  { "From The Start",                1 },  
  { "Misty",                         4 },  
  { "Serendipity",                   0 },
  { "Letter To My 13 Year Old Self", 1 },  
  { "Bewitched",                     3 },  
};

static int currentAnim  = 0;
static int currentFrame = 0;
static unsigned long lastFrameTime = 0;

void laufeyBegin(Adafruit_SSD1306& display) {
  _display = &display;
}

void laufeySetAnim(int index) {
  if (index >= 0 && index < 5) {
    currentAnim  = index;
    currentFrame = 0;
  }
}

void laufeyUpdate() {
  if (!_display) return;
  unsigned long now = millis();
  Animation& anim = animations[currentAnim];
  if (now - lastFrameTime >= (unsigned long)anim.frameDelay) {
    lastFrameTime = now;
    _display->clearDisplay();
    _display->drawBitmap(0, 0, anim.frames[currentFrame], 128, 64, SSD1306_WHITE);
    _display->display();
    currentFrame = (currentFrame + 1) % anim.frameCount;
  }
}

void laufeySong(const char* song) {
  int n = sizeof(songList) / sizeof(songList[0]);
  for (int i = 0; i < n; i++) {
    if (strstr(song, songList[i].songName) != NULL) {
      if (currentAnim != songList[i].animIndex) {
        currentAnim  = songList[i].animIndex;
        currentFrame = 0;
      }
      return;
    }
  }
}
