#include "global.h"

bool deviceConnected = false;

void ServerCallbacks ::onConnect(BLEServer *pServer)
{
  deviceConnected = true;
  digitalWrite(BUILTIN_LED,HIGH);
}

void ServerCallbacks ::onDisconnect(BLEServer *pServer)
{
  deviceConnected = false;
  digitalWrite(BUILTIN_LED,LOW);
}
