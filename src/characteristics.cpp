#include "global.h"

BLECharacteristic *pCharacteristicLEDNum;
BLECharacteristic *pCharacteristicTurnedOn;
BLECharacteristic *pCharacteristicBrightness;
BLECharacteristic *pCharacteristicColor;
BLECharacteristic *pCharacteristicMode;
bool isTurnedOn = true;
uint8_t brightnessLedStrip = 100;
int color = 0;
int newColor = 0;
int dec = 0x030303;
uint8_t mode = 0;

void CharacteristicTurnOnOffCallbacks ::onWrite(BLECharacteristic *pCharacteristic)
{
  Serial.println("CharacteristicTurnOnOffCallbacks");
  std::string rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0)
  {
    Serial.print("Received value: ");
    for (int i = 0; i < rxValue.length(); i++)
    {
      Serial.print(rxValue[i]);
    }

    if (rxValue.find("true") != -1)
    {

      isTurnedOn = true;
      //TODO
      // strip.clear();
      // strip.show();
    }
    else if (rxValue.find("false") != -1)
    {
      FastLED.clear();
      FastLED.show();
      isTurnedOn = false;
      //TODO
      // strip.clear();
      // strip.show();
    }
  }
}

void CharacteristicBrightnessCallbacks ::onWrite(BLECharacteristic *pCharacteristic)
{
  Serial.println("CharacteristicBrightnessCallbacks");
  std::string rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0)
  {
    Serial.print("Received value: ");
    for (int i = 0; i < rxValue.length(); i++)
    {
      Serial.print(rxValue[i]);
    }
    //TODO

    // brightnessLedStrip = atoi(rxValue.c_str());
    // strip.setBrightness(brightnessLedStrip);
    // strip.show();
  }
}

void CharacteristicColorCallbacks ::onWrite(BLECharacteristic *pCharacteristic)
{
  Serial.println("CharacteristicColorCallbacks");
  std::string rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0)
  {
    Serial.print("Received value: ");
    for (int i = 0; i < rxValue.length(); i++)
    {
      Serial.print(rxValue[i]);
    }
    newColor = atoi(rxValue.c_str());

    Serial.println(newColor);
    if (newColor > color)
    {
      dec = abs(dec);
    }
    else
    {
      dec = -abs(dec);
    }
  }
}

void CharacteristicModeCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
  Serial.println("CharacteristicModeCallbacks");
  std::string rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0)
  {
    Serial.print("Received value: ");
    for (int i = 0; i < rxValue.length(); i++)
    {
      Serial.print(rxValue[i]);
    }
    mode = atoi(rxValue.c_str());
    Serial.println(mode);
  }
}

void CharacteristicLEDNumCallbacks ::onRead(BLECharacteristic *pCharacteristic)
{
  Serial.println("Read NUM of leds");
  // std::ostringstream os;
  // os << NUM_LEDS;
  // pCharacteristic->setValue(os.str());
}