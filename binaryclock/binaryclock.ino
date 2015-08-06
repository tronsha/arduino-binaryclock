#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

int latchPin = 8;
int clockPinH = 2;
int dataPinH = 3;
int clockPinM = 4;
int dataPinM = 5;
int clockPinS = 6;
int dataPinS = 7;

void setup () {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPinS, OUTPUT);
  pinMode(dataPinS, OUTPUT); 
  pinMode(clockPinM, OUTPUT);
  pinMode(dataPinM, OUTPUT);
  pinMode(clockPinH, OUTPUT);
  pinMode(dataPinH, OUTPUT);
  Serial.begin(57600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin();
#endif
  rtc.begin();
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop () {
  DateTime now = rtc.now();
  digitalWrite(latchPin, LOW);
  shiftOut(dataPinS, clockPinS, MSBFIRST, now.second());
  shiftOut(dataPinM, clockPinM, MSBFIRST, now.minute()); 
  shiftOut(dataPinH, clockPinH, MSBFIRST, now.hour());
  digitalWrite(latchPin, HIGH);  
  delay(1000);
}
