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

void setup () {
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
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop () {
  DateTime now = rtc.now();
  digitalWrite(pinLatch, LOW);
  shiftOut(pinDataS, pinClockS, MSBFIRST, now.second());
  shiftOut(pinDataM, pinClockM, MSBFIRST, now.minute()); 
  shiftOut(pinDataH, pinClockH, MSBFIRST, now.hour());
  digitalWrite(pinLatch, HIGH);  
  delay(1000);
}
