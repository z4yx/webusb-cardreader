#ifndef RFID_H__
#define RFID_H__

#include "Arduino.h"


class RFID
{
    uint8_t uid[8];  // Buffer to store the returned UID
    uint8_t uidLength;                    // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    uint8_t card;
    char studentId[11], name[21], gender;
    bool found, skeletonKeyFound;
    unsigned long previousPollMillis;
    void printHexArray(uint8_t* data, int len);
public:
    enum{Card_None, Card_14443A, Card_14443B};
    RFID():uidLength(0),previousPollMillis(0),card(Card_None),found(false),skeletonKeyFound(false){}
    void Init();
    void Poll();
    bool Found()
    {
        return found;
    }
    char *GetStuID() { return studentId; }
    char *GetName() { return name; }
    char GetGender() { return gender; }
    uint8_t GetCardType() { return card; }
    uint8_t* GetUid(uint8_t &len)
    {
        len = uidLength;
        return uid;
    }
    void Next()
    {
        found = false;
        skeletonKeyFound = false;
    }
    void SetLedOn(bool on); 
};

#endif
