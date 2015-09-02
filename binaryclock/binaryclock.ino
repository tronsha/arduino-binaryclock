#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

int pinLatch = 8;
int pinClockH = 2;
int pinDataH = 3;
int pinClockM = 4;
int pinDataM = 5;
int pinClockS = 6;
int pinDataS = 7;

int pinSelect = 10;
int pinSetup = 11;

int count = 0;
int mode = 0b1000;

void setup () {
  pinMode(pinSelect, INPUT);
  pinMode(pinSetup, INPUT);
  pinMode(pinLatch, OUTPUT);
  pinMode(pinClockS, OUTPUT);
  pinMode(pinDataS, OUTPUT);
  pinMode(pinClockM, OUTPUT);
  pinMode(pinDataM, OUTPUT);
  pinMode(pinClockH, OUTPUT);
  pinMode(pinDataH, OUTPUT);
  Serial.begin(57600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin();
#endif
  rtc.begin();
  if (! rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop () {
  DateTime now = rtc.now();
  digitalWrite(pinLatch, LOW);

  if (digitalRead(pinSelect) == LOW) {
    count = 10;
    mode <<= 1;
    if (mode > 0b0100) {
      mode = 0b0001;
    }
  }

  if (digitalRead(pinSetup) == LOW) {
    count = 10;
    if (mode == 0b0001) {
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour() == 23 ? 0 : now.hour() + 1, now.minute(), now.second()));
    }
    if (mode == 0b0010) {
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute() == 59 ? 0 : now.minute() + 1, now.second()));
    }
    if (mode == 0b0100) {
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second() == 59 ? 0 : now.second() + 1));
    }
    now = rtc.now();
  }

  if (count > 0) {
    count--;
    shiftOut(pinDataH, pinClockH, MSBFIRST, mode);
    shiftOut(pinDataM, pinClockM, MSBFIRST, 0);
    if (mode == 0b0001) {
      shiftOut(pinDataS, pinClockS, MSBFIRST, now.hour());
    }
    if (mode == 0b0010) {
      shiftOut(pinDataS, pinClockS, MSBFIRST, now.minute());
    }
    if (mode == 0b0100) {
      shiftOut(pinDataS, pinClockS, MSBFIRST, now.second());
    }
  } else {
    mode = 0b1000;
    shiftOut(pinDataS, pinClockS, MSBFIRST, now.second());
    shiftOut(pinDataM, pinClockM, MSBFIRST, now.minute());
    shiftOut(pinDataH, pinClockH, MSBFIRST, now.hour());
  }

  digitalWrite(pinLatch, HIGH);
  delay(1000);
}
