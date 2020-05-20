#include "global.h"
#include <GradientPalettes.h>

////////////////// FastLED ////////////////////////

CRGB leds[NUM_LEDS];

const uint8_t brightnessCount = 5;
uint8_t brightnessMap[brightnessCount] = {16, 32, 64, 128, 255};
uint8_t brightnessIndex = 0;

uint8_t secondsPerPalette = 10;

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
uint8_t cooling = 49;

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
uint8_t sparking = 60;
uint8_t speed = 30;
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];

uint8_t gCurrentPaletteNumber = 0;

CRGBPalette16 gCurrentPalette(CRGB::Black);
CRGBPalette16 gTargetPalette(gGradientPalettes[0]);

CRGBPalette16 IceColors_p = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

uint8_t autoplay = 1;

uint8_t autoplayDuration = 10;
unsigned long autoPlayTimeout = 0;

uint8_t currentPaletteIndex = 0;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

CRGB solidColor = CRGB::Blue;

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct
{
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

void addGlitter(uint8_t chanceOfGlitter)
{
  if (random8() < chanceOfGlitter)
  {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

#include "Twinkles.h"
#include "TwinkleFOX.h"

void rainbow();
void rainbowWithGlitter();
void rainbowSolid();
void confetti();
void sinelon();
void bpm();
void juggle();
void fire();
void water();
void showSolidColor();
void pride();
void colorWaves();
void heatMap(CRGBPalette16 palette, bool up);
void colorwaves(CRGB *ledarray, uint16_t numleds, CRGBPalette16 &palette);
void adjustPattern(bool up);
void loadSettings();
byte power;
byte brightness;

PatternAndNameList patterns = {
    {pride, "Pride"},
    {colorWaves, "Color Waves"},

    // // twinkle patterns
    {rainbowTwinkles, "Rainbow Twinkles"},
    {snowTwinkles, "Snow Twinkles"},
    {cloudTwinkles, "Cloud Twinkles"},
    {incandescentTwinkles, "Incandescent Twinkles"},

    // // TwinkleFOX patterns
    {retroC9Twinkles, "Retro C9 Twinkles"},
    {redWhiteTwinkles, "Red & White Twinkles"},
    {blueWhiteTwinkles, "Blue & White Twinkles"},
    {redGreenWhiteTwinkles, "Red, Green & White Twinkles"},
    {fairyLightTwinkles, "Fairy Light Twinkles"},
    {snow2Twinkles, "Snow 2 Twinkles"},
    {hollyTwinkles, "Holly Twinkles"},
    {iceTwinkles, "Ice Twinkles"},
    {partyTwinkles, "Party Twinkles"},
    {forestTwinkles, "Forest Twinkles"},
    {lavaTwinkles, "Lava Twinkles"},
    {fireTwinkles, "Fire Twinkles"},
    {cloud2Twinkles, "Cloud 2 Twinkles"},
    {oceanTwinkles, "Ocean Twinkles"},

    {rainbow, "Rainbow"},
    {rainbowWithGlitter, "Rainbow With Glitter"},
    {rainbowSolid, "Solid Rainbow"},
    {confetti, "Confetti"},
    {sinelon, "Sinelon"},
    {bpm, "Beat"},
    {juggle, "Juggle"},
    {fire, "Fire"},
    {water, "Water"},

    {showSolidColor, "Solid Color"}};

const uint8_t patternCount = ARRAY_SIZE(patterns);

typedef struct
{
  CRGBPalette16 palette;
  String name;
} PaletteAndName;
typedef PaletteAndName PaletteAndNameList[];

const CRGBPalette16 palettes[] = {
    RainbowColors_p,
    RainbowStripeColors_p,
    CloudColors_p,
    LavaColors_p,
    OceanColors_p,
    ForestColors_p,
    PartyColors_p,
    HeatColors_p};

const uint8_t paletteCount = ARRAY_SIZE(palettes);

const String paletteNames[paletteCount] = {
    "Rainbow",
    "Rainbow Stripe",
    "Cloud",
    "Lava",
    "Ocean",
    "Forest",
    "Party",
    "Heat",
};

#include "effects.h"
//////////////////////////////////////////////////

uint8_t value = 0;
char tmpString[40];
int64_t notifyTime = 0;

void initStrip()
{
  FastLED.addLeds<LED_TYPE, PIN, COLOR_ORDER>(leds, NUM_LEDS); // for WS2812 (Neopixel)
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS); // for APA102 (Dotstar)
  FastLED.setDither(false);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightnessLedStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void initBLE()
{
  // Create BLE device:
  BLEDevice::init(DEVICE_NAME);

  // Create BLE server:
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  // Create BLE service:
  BLEService *pService = pServer->createService(SERVICE_UUID);
  Serial.println(pService->getUUID().toString().c_str());

  //Add NUM_LED characteristics
  pCharacteristicLEDNum = pService->createCharacteristic(
      CHARACTERISTIC_LED_NUM_UUID,
      BLECharacteristic::PROPERTY_READ);
  sprintf(tmpString, "%d", NUM_LEDS);
  pCharacteristicLEDNum->setValue(tmpString);
  pCharacteristicLEDNum->addDescriptor(new BLE2902());

  //Add Turn On/Off characteristics
  pCharacteristicTurnedOn = pService->createCharacteristic(
      CHARACTERISTIC_TURNED_ON_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
  sprintf(tmpString, "%s", isTurnedOn ? "true" : "false");
  pCharacteristicTurnedOn->setValue(tmpString);
  pCharacteristicTurnedOn->setCallbacks(new CharacteristicTurnOnOffCallbacks());
  pCharacteristicTurnedOn->addDescriptor(new BLE2902());

  //Add Brightness characteristics
  pCharacteristicBrightness = pService->createCharacteristic(
      CHARACTERISTIC_BRIGHTNESS_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
  sprintf(tmpString, "%d", brightnessLedStrip);
  pCharacteristicBrightness->setValue(tmpString);
  pCharacteristicBrightness->setCallbacks(new CharacteristicBrightnessCallbacks());
  pCharacteristicBrightness->addDescriptor(new BLE2902());

  //Add Color characteristics
  pCharacteristicColor = pService->createCharacteristic(
      CHARACTERISTIC_COLOR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
  sprintf(tmpString, "%d", color);
  pCharacteristicColor->setValue(tmpString);
  pCharacteristicColor->setCallbacks(new CharacteristicColorCallbacks());
  pCharacteristicColor->addDescriptor(new BLE2902());

  //Add Mode characteristics
  pCharacteristicMode = pService->createCharacteristic(
      CHARACTERISTIC_MODE_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
  sprintf(tmpString, "%d", mode);
  pCharacteristicMode->setValue(tmpString);
  pCharacteristicMode->setCallbacks(new CharacteristicModeCallbacks());
  pCharacteristicMode->addDescriptor(new BLE2902());

  // run service:
  pService->start();

  // run notification (advertising):
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

int64_t waiterTime = 0;

void wait(uint8_t milliseconds)
{
  waiterTime = millis();
  while (true)
  {
    if ((millis() - milliseconds) > waiterTime)
    {
      break;
    }
  }
}

//TODO
//WATCH DOG

void setup()
{
  delay(1000);

  color = 0x64B5F6;
  newColor = color;

  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(GPIO_NUM_2, OUTPUT);
  digitalWrite(GPIO_NUM_2, LOW);

  initStrip();
  initBLE();
}

void loop()
{

  if (deviceConnected)
  {
    // pCharacteristicTurnedOn->setValue(&value, 1);
    //pCharacteristic->indicate();
    if ((millis() - 2000) > notifyTime)
    {
      Serial.printf("*** NOTIFY: %d ***\n", value);

      notifyTime = millis();
      pCharacteristicTurnedOn->indicate();
      pCharacteristicTurnedOn->notify();
      value++;
    }
  }

  random16_add_entropy(random(65535));

  EVERY_N_SECONDS(secondsPerPalette)
  {
    gCurrentPaletteNumber = addmod8(gCurrentPaletteNumber, 1, gGradientPaletteCount);
    gTargetPalette = gGradientPalettes[gCurrentPaletteNumber];
  }

  EVERY_N_MILLISECONDS(40)
  {
    nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 8);
    gHue++; // slowly cycle the "base color" through the rainbow
  }

  if (isTurnedOn)
  {
    patterns[mode].pattern();
    FastLED.show();
    FastLED.delay(1000 / FRAMES_PER_SECOND);
  }
}