#include "laufey.h"
#include "bitmaps.h"

static Adafruit_SSD1306* _display = nullptr;

struct Animation {
  const char* name;
  const uint8_t* frames[10];
  int frameCount;
  int frameDelay;
};

static Animation animations[] = {
  { "laufeydurmiendo", { laufeysleepsi, laufeysleepsi2, laufeysleepsi3 }, 3, 400 },
  { "laufeychiquita",  { laufeychiquita1, laufeychic2, launfeychic3 },    3, 300 },
  { "meimei",          { meimei1, meimei2, meimei3 },                     3, 200 }
};

struct SongMap { const char* songName; int animIndex; };

static SongMap songList[] = {
  { "Dreamer",                       2 },
  { "Second Best",                   0 },
  { "Haunted",                       0 },
  { "Must Be Love",                  1 },
  { "While You Were Sleeping",       0 },
  { "Lovesick",                      1 },
  { "California and Me",             2 },
  { "Nocturne (Interlude)",          0 },
  { "Promise",                       1 },
  { "From The Start",                1 },
  { "Misty",                         2 },
  { "Serendipity",                   2 },
  { "Letter To My 13 Year Old Self", 2 },
  { "Bewitched",                     1 }
};

static int currentAnim  = 0;
static int currentFrame = 0;
static unsigned long lastFrameTime = 0;

void laufeyBegin(Adafruit_SSD1306& display) {
  _display = &display;
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
