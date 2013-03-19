#include <OctoWS2811.h>
#include "triangular.h"
#include "color.h"

using namespace Triangular;

static const int ledCount = 16;
DMAMEM int displayMemory[ledCount*6];
int drawingMemory[ledCount*6];
OctoWS2811 leds(ledCount, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz);

// This table describes the way the LED strips are arragned in the 16-pixel triangular grid.
static const uint16_t grid16_lut[] =
{
  /*  0 */  Grid::EVEN | Grid::DIR_POS |                 Grid::NBR_POS | Grid::V_NONE,
  /*  1 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 11,
  /*  2 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | Grid::V_NONE,
  /*  3 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 9,
  /*  4 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | Grid::V_NONE,
  /*  5 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 7,
  /*  6 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG |                 Grid::V_NONE,

  /*  7 */  Grid::EVEN | Grid::DIR_NEG | Grid::NBR_NEG |                 5,
  /*  8 */  Grid::ODD  | Grid::DIR_NEG | Grid::NBR_NEG | Grid::NBR_POS | 14,
  /*  9 */  Grid::EVEN | Grid::DIR_NEG | Grid::NBR_NEG | Grid::NBR_POS | 3,
  /* 10 */  Grid::ODD  | Grid::DIR_NEG | Grid::NBR_NEG | Grid::NBR_POS | 12,
  /* 11 */  Grid::EVEN | Grid::DIR_NEG |                 Grid::NBR_POS | 1,

  /* 12 */  Grid::EVEN | Grid::DIR_POS |                 Grid::NBR_POS | 10,
  /* 13 */  Grid::ODD  | Grid::DIR_POS | Grid::NBR_NEG | Grid::NBR_POS | 15,
  /* 14 */  Grid::EVEN | Grid::DIR_POS | Grid::NBR_NEG |                 8,

  /* 15 */  Grid::EVEN | Grid::DIR_NEG |                                 13,
};  
Grid grid(grid16_lut, sizeof grid16_lut);

static const Point center(9);

// Wandering point, starts at origin.
Point p = center;
int counter = 0;

void setup()
{
  leds.begin();
}

void loop()
{
  // Background
  for (unsigned i = 0; i < ledCount; ++i) {
    Point p = i;
    unsigned dist = grid.distance(p, center);
    leds.setPixel(i, lerp(0x000000, 0x200820, (sin(dist * 0.2 + millis() * 0.001) + 1) * 127));
  }

  // Wanderer
  leds.setPixel(p, 0xffffff);
  if (++counter == 100) {
    counter = 0;
    grid.move(p, random(1, 4));
  }

  // Shape
  Point z;
  if ((z = grid.next(p, A_AXIS)) != Point::invalid()) leds.setPixel(z, 0xff0000);
  if ((z = grid.next(p, B_AXIS)) != Point::invalid()) leds.setPixel(z, 0x00ff00);
  if ((z = grid.next(p, C_AXIS)) != Point::invalid()) leds.setPixel(z, 0x0000ff);

  leds.show();
  delay(5);
}
