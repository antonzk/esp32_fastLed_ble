#include <Arduino.h>
#include <sstream>

#define FASTLED_INTERRUPT_RETRY_COUNT 0

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <FastLED.h>
FASTLED_USING_NAMESPACE

///// FastLED //////////

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define LED_TYPE      WS2812 // Type 
#define COLOR_ORDER   GRB

#define MILLI_AMPS         2000 
#define FRAMES_PER_SECOND  120  

//////////////////////////////////


#define PIN T4 // P13 - data pin
#define DEVICE_NAME "IoT LED Strip" // Bluetooth device name
#define NUM_LEDS 150 // amount of leds in strip

#define SERVICE_UUID "97727c46-0371-11ea-8d71-362b9e155667"



//LED CHARACTERISTICS

//NUMBER OF LEDS
#define CHARACTERISTIC_LED_NUM_UUID "3ec104d6-0390-11ea-8d71-362b9e155667"
extern BLECharacteristic *pCharacteristicLEDNum;


//TURNED ON/OFF
#define CHARACTERISTIC_TURNED_ON_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
extern BLECharacteristic *pCharacteristicTurnedOn;
extern bool isTurnedOn;

//BRIGHTNESS
#define CHARACTERISTIC_BRIGHTNESS_UUID "c7b35a64-0372-11ea-8d71-362b9e155667"
extern BLECharacteristic *pCharacteristicBrightness;
extern uint8_t brightnessLedStrip;

//COLOR
#define CHARACTERISTIC_COLOR_UUID "ced9abe0-0372-11ea-8d71-362b9e155667"
extern BLECharacteristic *pCharacteristicColor;
extern int color;
extern int newColor;
extern int dec;

//MODE
#define CHARACTERISTIC_MODE_UUID "d8257f80-0372-11ea-8d71-362b9e155667"
extern BLECharacteristic *pCharacteristicMode;
extern uint8_t mode;

////////////////////////////////////////////
extern bool deviceConnected;


class CharacteristicLEDNumCallbacks : public BLECharacteristicCallbacks
{
public:
  void onRead(BLECharacteristic *pCharacteristic);
};

class CharacteristicTurnOnOffCallbacks : public BLECharacteristicCallbacks
{
public:
  void onWrite(BLECharacteristic *pCharacteristic);
};

class CharacteristicBrightnessCallbacks : public BLECharacteristicCallbacks
{
public:
  void onWrite(BLECharacteristic *pCharacteristic);
};

class CharacteristicColorCallbacks : public BLECharacteristicCallbacks
{
public:
  void onWrite(BLECharacteristic *pCharacteristic);
};

class CharacteristicModeCallbacks : public BLECharacteristicCallbacks
{
public:
  void onWrite(BLECharacteristic *pCharacteristic);
};



class ServerCallbacks : public BLEServerCallbacks
{
public:
  void onConnect(BLEServer *pServer);
  void onDisconnect(BLEServer *pServer);
};