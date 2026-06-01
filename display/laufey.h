#pragma once
#include <Adafruit_SSD1306.h>

void laufeyBegin(Adafruit_SSD1306& display);
void laufeyUpdate();
void laufeySong(const char* songName);
