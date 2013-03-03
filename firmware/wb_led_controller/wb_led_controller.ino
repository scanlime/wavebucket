#include <OctoWS2811.h>

const int ledsPerStrip = 240;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, WS2811_800kHz);

void setup() {
  Serial.setTimeout(50);
  leds.begin();
  leds.show();
}

void loop() {
  // Start char
  if (Serial.read() == 'U') {
    int count = Serial.readBytes((char *)drawingMemory, sizeof(drawingMemory));
    if (count == sizeof(drawingMemory)) {
      // Flow control
      Serial.write('*');
      leds.show();
    }
  }
}

