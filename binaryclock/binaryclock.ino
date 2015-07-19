#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

int latchPinMinute = 8;
int clockPinMinute = 12;
int dataPinMinute = 11;
int latchPinHour = 2;
int clockPinHour = 7;
int dataPinHour = 6;

void setup () {
  pinMode(latchPinMinute, OUTPUT);
  pinMode(clockPinMinute, OUTPUT);
  pinMode(dataPinMinute, OUTPUT);
  pinMode(latchPinHour, OUTPUT);
  pinMode(clockPinHour, OUTPUT);
  pinMode(dataPinHour, OUTPUT);
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
  digitalWrite(latchPinMinute, LOW);
  digitalWrite(latchPinHour, LOW);
  shiftOut(dataPinMinute, clockPinMinute, MSBFIRST, now.minute()); 
  shiftOut(dataPinHour, clockPinHour, MSBFIRST, now.hour());
  digitalWrite(latchPinMinute, HIGH);
  digitalWrite(latchPinHour, HIGH);
  delay(3000);
}
