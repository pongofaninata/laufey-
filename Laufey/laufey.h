#pragma once
#include <Adafruit_SSD1306.h>

void laufeyBegin(Adafruit_SSD1306& display);
void laufeyUpdate();
void laufeySong(const char* songName);
void laufeySetAnim(int index);  // 0=sleep 1=chiquita 2=meimei 3=vinil 4=faro
