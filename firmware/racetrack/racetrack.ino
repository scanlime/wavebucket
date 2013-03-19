#include <OctoWS2811.h>

const int ledsPerStrip = 240;
const int ledCount = 240;
const int numStars = 15;
const int reversePercent = 20;

struct Star {
  float pos;
  float velocity;
  int color;
} stars[numStars];

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz);

int lerp(int c1, int c2, int alpha) {
  // Linear interpolate, with alpha in range [0, 256].
  // Returns c1 if alpha==0, or c2 if alpha==256.
  int r1 = c1 & 0xFF0000;
  int g1 = c1 & 0x00FF00;
  int b1 = c1 & 0x0000FF;
  int r2 = c2 & 0xFF0000;
  int g2 = c2 & 0x00FF00;
  int b2 = c2 & 0x0000FF;
  int invA = 0x100 - alpha;
  return ((((r1 * invA) + (r2 * alpha)) >> 8) & 0xFF0000) |
         ((((g1 * invA) + (g2 * alpha)) >> 8) & 0x00FF00) |
         ((((b1 * invA) + (b2 * alpha)) >> 8) & 0x0000FF) ;
}

int addsat(int c1, int c2) {
  int r1 = c1 & 0xFF0000;
  int g1 = c1 & 0x00FF00;
  int b1 = c1 & 0x0000FF;
  int r2 = c2 & 0xFF0000;
  int g2 = c2 & 0x00FF00;
  int b2 = c2 & 0x0000FF;
  r1 += r2;
  g1 += g2;
  b1 += b2;
  if (r1 & 0xFF000000) r1 = 0xFF0000;
  if (g1 & 0xFFFF0000) g1 = 0x00FF00;
  if (b1 & 0xFFFFFF00) b1 = 0x0000FF;
  return r1 | g1 | b1;
}

void initStar(int i) {
  stars[i].pos = 0;
  stars[i].velocity = (100 + random(8000)) / 10000.0;
  stars[i].color = random(0x1000000) | 0x101010;

  if (random(100) < reversePercent) {
     // Reverse
     stars[i].pos = ledCount - 1;
     stars[i].velocity = -stars[i].velocity;
  }
}

void ledDecay(int i) {
  leds.setPixel(i, lerp(leds.getPixel(i), 0, 40));
}

void updateStar(int i) {
  stars[i].pos += stars[i].velocity;

  int iPos = stars[i].pos;
  int fpPos = int(stars[i].pos * 256) & 0xFF;

  if (iPos >= ledCount || iPos < 0) {
    initStar(i);
  } else {
    // subpixel interpolate
    leds.setPixel(iPos, addsat(leds.getPixel(iPos), lerp(stars[i].color, 0, fpPos)));
    if (fpPos && iPos + 1 < ledCount) {
      leds.setPixel(iPos+1, addsat(leds.getPixel(iPos+1), lerp(0, stars[i].color, fpPos)));
    }
  }
}

void setup() {
  leds.begin();

  for (int s = 0; s < numStars; ++s) {
    initStar(s);
  }
}

void loop() {
  for (int i = 0; i < ledCount; ++i) {
    ledDecay(i);
  }
    
  for (int s = 0; s < numStars; ++s) {
    updateStar(s);
  }

  leds.show();
}

