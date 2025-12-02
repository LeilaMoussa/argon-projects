/*
 * Example of using the ChainableRGB library for controlling a Grove RGB.
 * This code cycles through all the colors in an uniform way. This is accomplished using a HSB color space.
 */

#include "Particle.h"

#include <Grove_ChainableLED.h>

SYSTEM_THREAD(ENABLED);

#define NUM_LEDS 1

ChainableLED leds(D4, D5, NUM_LEDS);

SerialLogHandler logHandler(LOG_LEVEL_INFO); // /dev/ttyACM0

int setRequestedColor(String cmd) {
  if (cmd == "red") {
    leds.setColorRGB(0, 255, 0, 0);
  } else if (cmd == "blue") {
    leds.setColorRGB(0, 0, 0, 255);
  } else if (cmd == "green") {
    leds.setColorRGB(0, 0, 255, 0);
  } else if (cmd == "white") {
    leds.setColorRGB(0, 255, 255, 255);
  } else if (cmd == "off") {
    leds.setColorRGB(0, 0, 0, 0);
  } else {
    Log.error("unexpected cmd %s", (const char*)cmd);
    return -1;
  }
  return 0;
}

void setup() {
  leds.init();
  Particle.function("setTheColor", setRequestedColor);
  Serial.begin();
  // Serial1.begin(9600);
}

float hue = 0.0;
boolean up = true;
void cycleThroughColors() {
  Log.info("Cycling through colors");
  for (byte i=0; i<NUM_LEDS; i++)
    leds.setColorHSB(i, hue, 1.0, 0.5);
  delay(50);
  if (up)
    hue+= 0.025;
  else
    hue-= 0.025;

  if (hue>=1.0 && up)
    up = false;
  else if (hue<=0.0 && !up)
    up = true;
}

//void loop() {
  //cycleThroughColors();
  //setRequestedColor("red");
//}

// Constants
const size_t READ_BUF_SIZE = 64;

// Forward declarations
// void processBuffer();

// Global variables
char readBuf[READ_BUF_SIZE];
size_t readBufOffset = 0;
void loop() {
  // Read data from serial
  while(Serial.available()) {
    if (readBufOffset < READ_BUF_SIZE) {
      char c = Serial.read();
      if (c != '\n') {
        // Add character to buffer
        readBuf[readBufOffset++] = c;
      }
      else {
        // End of line character found, process line
        readBuf[readBufOffset] = 0;
        // processBuffer();
        setRequestedColor(String(readBuf));
        readBufOffset = 0;
      }
    }
    else {
      readBufOffset = 0;
    }
  }

}
