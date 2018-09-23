#include <WebUSB.h>
#include "rfid.h"

/**
 * Creating an instance of WebUSBSerial will add an additional USB interface to
 * the device that is marked as vendor-specific (rather than USB CDC-ACM) and
 * is therefore accessible to the browser.
 *
 * The URL here provides a hint to the browser about what page the user should
 * navigate to to interact with the device.
 */
WebUSB WebUSBSerial(1 /* https:// */, "join.tuna.tsinghua.edu.cn");

RFID rfid;

#define Serial WebUSBSerial

void setup() {
  while (!Serial) {
    ;
  }
  Serial.begin(9600);
  Serial.println("Sketch begins.");
  rfid.Init();
  Serial.flush();
}

double lastKeepalive;
void loop() {
  if (Serial && Serial.available()) {
    int byte = Serial.read();
    if (byte == 'H') {
      Serial.println("Turning LED on.");
    } else if (byte == 'L') {
      Serial.println("Turning LED off.");
    }
    Serial.flush();
  }
  rfid.Poll();
  if(rfid.Found()){
    if(rfid.GetCardType() == RFID::Card_14443B){
      Serial.println("CardBegin");
      Serial.print("StuNumber="); Serial.println(rfid.GetStuID());
      Serial.print("Name="); Serial.println(rfid.GetName());
      Serial.print("Gender="); Serial.println(rfid.GetGender());
      Serial.println("CardEnd");
      Serial.flush();
    }
    rfid.Next();
    Serial.flush();
  }
  if(millis()-lastKeepalive > 1000){
    Serial.println("KeepAlive");
    Serial.flush();
    lastKeepalive = millis();
  }
}
