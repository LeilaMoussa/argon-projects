/*
 * Example of using the ChainableRGB library for controlling a Grove RGB.
 * This code cycles through all the colors in an uniform way. This is accomplished using a HSB color space.
 */

#include "Particle.h"
#include <Grove_ChainableLED.h>
#include <Debounce.h>

//SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

#define NUM_LEDS 1
#define BUZZER_PIN D2
#define BUTTON_PIN A2
#define FSM_TICK 500

ChainableLED leds(D4, D5, NUM_LEDS);

SerialLogHandler logHandler(LOG_LEVEL_INFO); // /dev/ttyACM0 or /dev/tty.usbmodem1101

// twinkle-twinkle
int length = 15;         /* the number of notes */
char notes[] = "ccggaagffeeddc ";
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 300;

int alarm_hour = 7;
int alarm_minute = 5;
float offset = -8;

static volatile int tickFlag = 0;
static volatile int buttonPushes = 0;

void playTone(int tone, int duration);
void playNote(char note, int duration);
void playMelody();
void checkTime();
int setRequestedColor(String cmd);
int setTzOffset(String cmd);
int setAlarmTime(String cmd);
void cycleThroughColors();
void setTickFlag();
void clearSelection();

Timer alarmTimer(60000, checkTime);
Timer fsmTimer(FSM_TICK, setTickFlag);
Timer timeoutTimer(10000, clearSelection);
Debounce debouncer = Debounce();

void setTickFlag() {
    tickFlag = 1;
}

void clearSelection() {
  Log.info("button pushes %d", buttonPushes);
  alarm_hour = buttonPushes;
  buttonPushes = 0;
  Log.info("alarm hour is %d", alarm_hour);
}

/* play tone */
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(tone);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(tone);
  }
}

void stopAlarm() {
  digitalWrite(BUZZER_PIN, LOW);
  setRequestedColor("off");
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
    cycleThroughColors();
    playMelody();
  }
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

int setTzOffset(String cmd) {
  offset = cmd.toInt();
  Time.zone(offset);
  return 0;
}

int setAlarmTime(String cmd) {
  // format hh:mm
  int hour = cmd.substring(0, 2).toInt();
  int minute = cmd.substring(3, 5).toInt();
  alarm_hour = hour;
  alarm_minute = minute;
  // need to publish this time to cloud, and fetch it on start
  return 0;
}

void setup() {
  leds.init();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  Time.zone(offset);
  alarmTimer.start();
  // BUG: This ISR causes the argon to crash, for some reason.
  // attachInterrupt(BUTTON_PIN, stopAlarm, RISING);
  fsmTimer.start();

  debouncer.attach(BUTTON1_PIN, INPUT_PULLDOWN);
  debouncer.interval(20);

  Particle.function("setColor", setRequestedColor);
  Particle.function("setTzOffset", setTzOffset);
  Particle.function("setAlarmTime", setAlarmTime);
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

// Constants
const size_t READ_BUF_SIZE = 64;

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

  debouncer.update();
  if (tickFlag) {
    if (debouncer.rose()) {
      //tone(BUZZER_PIN, 100, 100);  
      buttonPushes++;
      Log.info("push %d", buttonPushes);
      tickFlag = 0;
      if (timeoutTimer.isActive()) {
        timeoutTimer.stop();
      }
      timeoutTimer.start();
    }
  }

}
