#include "rfid.h"
#include <Wire.h>
#include <WebUSB.h>
#include <PN532_HSU.h>
#include "PN532.h"

extern WebUSB WebUSBSerial;

#define Serial WebUSBSerial

static PN532_HSU pn532_hsu(Serial1);
static PN532 pn532(pn532_hsu);

const int ledPin = 13;

void RFID::printHexArray(uint8_t* data, int len)
{
    for (uint8_t i=0; i < len; i++) 
    {
      Serial.print(data[i], HEX); 
      Serial.print(' ');
    }
    Serial.println("");

}

void RFID::Poll()
{
  boolean success;

  if(Found()){ //A card had been found previously
    return;
  }
  
  unsigned long currentMillis = millis();
  if(currentMillis - previousPollMillis > 100){
    previousPollMillis = currentMillis;
  }else{
    return;
  }

  if(card == Card_14443B){
    if(pn532.stuCardIsPresent()){ //Card doesn't leave yet
      return;
    }else{
      pn532.resetConfigFor14443B();
      card = Card_None;
      SetLedOn(false);
    }
  }

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 200, true);
  if(card == Card_14443A){
    if(success){
      //Card doesn't leave yet since last detected
      pn532.inRelease(0); //Release all cards
      return;
    }else{
      card = Card_None;
      SetLedOn(false);
    }
  }

  if (success) {
    // Serial.println("Found TypeA card!");
    // Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    // Serial.print("UID Value: ");
    // printHexArray(uid, uidLength);
    // Serial.println("");
    pn532.inRelease(0); //Release all cards
    card = Card_14443A;
    found = true;
    SetLedOn(true);
  }
  else
  {
    // PN532 probably timed out waiting for a card
    //Serial.println("Timed out waiting for a card");
  }

  static uint8_t AFI[] = {0};
  success = pn532.inListPassiveTarget(PN532_106KBPS_ISO14443B, sizeof(AFI) , AFI, 200);
  if (success) {
    uint8_t expire[3]; 

    pn532.inRelease();
    success = pn532.readTsighuaStuCard(uid, expire, studentId, &gender, name);
    if(success){
      // Serial.println("Found student card!");
      // Serial.print("Student Number: "); Serial.println(studentId);
      card = Card_14443B;
      uidLength = 3;
      found = true;
      SetLedOn(true);
    }else{
      pn532.resetConfigFor14443B();
    }
  }else{

  }
}

void RFID::Init()
{
  pinMode(ledPin, OUTPUT);
  pn532.begin();

  uint32_t versiondata = pn532.getFirmwareVersion();
  if (! versiondata) {
    Serial.println("Didn't find PN53x board");
    return;
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.print((versiondata>>24) & 0xFF, HEX); 
  Serial.print(" with firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  pn532.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  pn532.SAMConfig();
}

void RFID::SetLedOn(bool on){
  if(on){
    digitalWrite(ledPin, HIGH);
    pn532.writeGPIOP7(0);
  }
  else{
    digitalWrite(ledPin, LOW);
    pn532.writeGPIOP7(1<<PN532_GPIO_P71);
  }
}
