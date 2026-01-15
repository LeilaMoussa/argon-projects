/*
 * Example of using the ChainableRGB library for controlling a Grove RGB.
 * This code cycles through all the colors in an uniform way. This is accomplished using a HSB color space.
 */

#include "Particle.h"

#include <Grove_ChainableLED.h>

//SYSTEM_THREAD(ENABLED);

#define NUM_LEDS 1
#define BUZZER_PIN D2

ChainableLED leds(D4, D5, NUM_LEDS);

SerialLogHandler logHandler(LOG_LEVEL_INFO); // /dev/ttyACM0

// twinkle-twinkle
int length = 15;         /* the number of notes */
char notes[] = "ccggaagffeeddc ";
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 300;

int alarm_hour = 7;
int alarm_minute = 0;
float offset = -8;

/* play tone */
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(tone);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
      if (names[i] == note) {
          playTone(tones[i], duration);
      }
  }
}

void playMelody() {
  for(int i = 0; i < length; i++) {
    if(notes[i] == ' ') {
      delay(beats[i] * tempo);
    } else {
      playNote(notes[i], beats[i] * tempo);
    }
    delay(tempo / 2);    /* delay between notes */
  }
}

void checkTime() {
  if (Time.hour() == alarm_hour && Time.minute() == alarm_minute) {
    playMelody();
  }
}

Timer timer(60000, checkTime);

int changeAlarmSettings(String cmd) {
  // format hour,minute,offset
  return 0;
}

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
  pinMode(BUZZER_PIN, OUTPUT);

  // on startup, diff between next alarm time and now, in millis
  // set alarm timer with that difference
  // initial=true
  // alarm fn plays melody
  // if initial is true, change time period to 24 hours
  // sets initial=false if it's true

  // need to call cloud to find out offset
  // actually, receive offset as optional input, do that later
  Time.zone(-8);
  timer.start();
  Particle.function("setColor", setRequestedColor);
  Serial.begin();
}

float hue = 0.0;
boolean up = true;
void cycleThroughColors() {
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
        setRequestedColor(String(readBuf));
        readBufOffset = 0;
      }
    }
    else {
      readBufOffset = 0;
    }
  }
}
