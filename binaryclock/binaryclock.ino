#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

void shiftOutTime(DateTime);
void shiftOutDate(DateTime);
void shiftOutSetup(DateTime, int);
void setupDateTime(DateTime, int);

int pinClock2 = 2;
int pinData2 = 3;
int pinClock1 = 4;
int pinData1 = 5;
int pinClock0 = 6;
int pinData0 = 7;
int pinLatch = 8;
int pinSelect = 10;
int pinSetup = 11;

int count = 0;
int mode = 0b01000000;

void setup () {
  pinMode(pinSelect, INPUT_PULLUP);
  pinMode(pinSetup, INPUT_PULLUP);
  pinMode(pinLatch, OUTPUT);
  pinMode(pinClock0, OUTPUT);
  pinMode(pinData0, OUTPUT);
  pinMode(pinClock1, OUTPUT);
  pinMode(pinData1, OUTPUT);
  pinMode(pinClock2, OUTPUT);
  pinMode(pinData2, OUTPUT);
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
    if (digitalRead(pinSetup) == LOW) {
      count = 10;
      setupDateTime(now, mode);
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
  shiftOut(pinData0, pinClock0, MSBFIRST, now.year() - 2000);
  shiftOut(pinData1, pinClock1, MSBFIRST, now.month());
  shiftOut(pinData2, pinClock2, MSBFIRST, now.day());
}

void shiftOutTime(DateTime now) {
  shiftOut(pinData0, pinClock0, MSBFIRST, now.second());
  shiftOut(pinData1, pinClock1, MSBFIRST, now.minute());
  shiftOut(pinData2, pinClock2, MSBFIRST, now.hour());
}

void shiftOutSetup(DateTime now, int mode) {
  shiftOut(pinData2, pinClock2, MSBFIRST, mode);
  shiftOut(pinData1, pinClock1, MSBFIRST, 0);
  if (mode == 0b00000001) {
    shiftOut(pinData0, pinClock0, MSBFIRST, now.hour());
  }
  if (mode == 0b00000010) {
    shiftOut(pinData0, pinClock0, MSBFIRST, now.minute());
  }
  if (mode == 0b00000100) {
    shiftOut(pinData0, pinClock0, MSBFIRST, now.second());
  }
  if (mode == 0b00001000) {
    shiftOut(pinData0, pinClock0, MSBFIRST, now.year() - 2000);
  }
  if (mode == 0b00010000) {
    shiftOut(pinData0, pinClock0, MSBFIRST, now.month());
  }
  if (mode == 0b00100000) {
    shiftOut(pinData0, pinClock0, MSBFIRST, now.day());
  }
}

void setupDateTime(DateTime now, int mode) {
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

