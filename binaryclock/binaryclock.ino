#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

void shiftOutTime(DateTime);
void shiftOutDate(DateTime);
void shiftOutSetup(DateTime, int);
void up(DateTime, int);
void down(DateTime, int);

const int pinSpeaker = 4;
const int pinClock = 6;
const int pinData = 7;
const int pinLatch = 8;
const int pinDown = 9;
const int pinUp = 10;
const int pinSetup = 11;
const int pinSelect = 12;

int count = 0;
int mode = 0b01000000;

void setup () {
  noTone(pinSpeaker);
  pinMode(pinDown, INPUT_PULLUP);
  pinMode(pinUp, INPUT_PULLUP);
  pinMode(pinSetup, INPUT_PULLUP);
  pinMode(pinSelect, INPUT_PULLUP);
  pinMode(pinLatch, OUTPUT);
  pinMode(pinClock, OUTPUT);
  pinMode(pinData, OUTPUT);
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
    if (mode > 0b00100000) {
      mode = 0b00000001;
    }
  }
  if (count > 0) {
    count--;
    if (digitalRead(pinUp) == LOW) {
      count = 10;
      up(now, mode);
      now = rtc.now();
    }
    if (digitalRead(pinDown) == LOW) {
      count = 10;
      down(now, mode);
      now = rtc.now();
    }
    shiftOutSetup(now, mode);
  } else {
    mode = 0b01000000;
    if (digitalRead(pinSetup) == LOW) {
      shiftOutDate(now);
    } else {
      shiftOutTime(now);
    }
  }
  digitalWrite(pinLatch, HIGH);
  delay(1000);
}

void shiftOutDate(DateTime now) {
  shiftOut(pinData, pinClock, MSBFIRST, now.day());
  shiftOut(pinData, pinClock, MSBFIRST, now.month());
  shiftOut(pinData, pinClock, MSBFIRST, now.year() - 2000);
}

void shiftOutTime(DateTime now) {
  shiftOut(pinData, pinClock, MSBFIRST, now.hour());
  shiftOut(pinData, pinClock, MSBFIRST, now.minute());
  shiftOut(pinData, pinClock, MSBFIRST, now.second());
}

void shiftOutSetup(DateTime now, int mode) {
  shiftOut(pinData, pinClock, MSBFIRST, mode);
  shiftOut(pinData, pinClock, MSBFIRST, 0);
  if (mode == 0b00000001) {
    shiftOut(pinData, pinClock, MSBFIRST, now.hour());
  }
  if (mode == 0b00000010) {
    shiftOut(pinData, pinClock, MSBFIRST, now.minute());
  }
  if (mode == 0b00000100) {
    shiftOut(pinData, pinClock, MSBFIRST, now.second());
  }
  if (mode == 0b00001000) {
    shiftOut(pinData, pinClock, MSBFIRST, now.year() - 2000);
  }
  if (mode == 0b00010000) {
    shiftOut(pinData, pinClock, MSBFIRST, now.month());
  }
  if (mode == 0b00100000) {
    shiftOut(pinData, pinClock, MSBFIRST, now.day());
  }
}

void up(DateTime now, int mode) {
  if (mode == 0b00000001) {
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour() == 23 ? 0 : now.hour() + 1, now.minute(), now.second()));
  }
  if (mode == 0b00000010) {
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute() == 59 ? 0 : now.minute() + 1, now.second()));
  }
  if (mode == 0b00000100) {
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second() == 59 ? 0 : now.second() + 1));
  }
  if (mode == 0b00001000) {
    rtc.adjust(DateTime(now.year() == 2060 ? 2015 : now.year() + 1, now.month(), now.day(), now.hour(), now.minute(), now.second()));
  }
  if (mode == 0b00010000) {
    rtc.adjust(DateTime(now.year(), now.month() == 12 ? 1 : now.month() + 1, now.day(), now.hour(), now.minute(), now.second()));
  }
  if (mode == 0b00100000) {
    rtc.adjust(DateTime(now.year(), now.month(), now.day() == 31 ? 1 : now.day() + 1, now.hour(), now.minute(), now.second()));
  }
}

void down(DateTime now, int mode) {
  if (mode == 0b00000001) {
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour() == 0 ? 23 : now.hour() - 1, now.minute(), now.second()));
  }
  if (mode == 0b00000010) {
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute() == 0 ? 59 : now.minute() - 1, now.second()));
  }
  if (mode == 0b00000100) {
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second() == 0 ? 59 : now.second() - 1));
  }
  if (mode == 0b00001000) {
    rtc.adjust(DateTime(now.year() == 2015 ? 2060 : now.year() - 1, now.month(), now.day(), now.hour(), now.minute(), now.second()));
  }
  if (mode == 0b00010000) {
    rtc.adjust(DateTime(now.year(), now.month() == 1 ? 12 : now.month() - 1, now.day(), now.hour(), now.minute(), now.second()));
  }
  if (mode == 0b00100000) {
    rtc.adjust(DateTime(now.year(), now.month(), now.day() == 1 ? 31 : now.day() - 1, now.hour(), now.minute(), now.second()));
  }
}

