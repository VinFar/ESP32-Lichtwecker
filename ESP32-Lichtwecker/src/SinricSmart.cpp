#include "SinricPro.h"
#include "SinricProLight.h"
#include "Debug.h"
#include "myLED.h"
#include "SinricCredentials.h"
#include "NeoPixel.h"

#if defined(APP_KEY) || defined(APP_SECRET) || defined(LIGHT_ID)
#else
#error "You need to define your Sinric Credentials"
#endif

#define DEBUG_MSG DEBUG_MSG_NEOPIXEL


// we use a struct to store all states and values for our light
struct {
  bool powerState = false;
  int brightness = 0;
} device_state; 

bool onPowerState(const String &deviceId, bool &state) {
  DEBUG_PRINT("Device %s power turned %s" CLI_NL, deviceId.c_str(), state?"on":"off");
  device_state.powerState = state;
  if(state){
    LedReadLightOn();
  }else{
    LedReadLightOff();
    NeoPixelAllOff();
  }
  return true; // request handled properly
}

bool onBrightness(const String &deviceId, int &brightness) {
  device_state.brightness = brightness;
  DEBUG_PRINT("Device %s brightness level changed to %d" CLI_NL, deviceId.c_str(), brightness);
  LedWakeSetDutyCycle((float)brightness);
  return true;
}

bool onAdjustBrightness(const String &deviceId, int brightnessDelta) {
  device_state.brightness += brightnessDelta;
  DEBUG_PRINT("Device %s brightness level changed about %i to %d" CLI_NL, deviceId.c_str(), brightnessDelta, device_state.brightness);
  brightnessDelta = device_state.brightness;
  LedWakeSetDutyCycle(LedPwmGet()+brightnessDelta);
  return true;
}

bool onSetColor(const String &deviceId, byte &R, byte &G, byte &B){
  DEBUG_PRINT("Device Color set to: R:%d, G:%d, B:%d" CLI_NL,R,G,B);
  NeoPixelsetAll(R,G,B);
  NeoPixelSetBrightness(255);
  return true;
}

void setupSinricPro() {
  // get a new Light device from SinricPro
  SinricProLight &myLight = SinricPro[LIGHT_ID];

  
  // set callback function to device
  myLight.onPowerState(onPowerState);
  myLight.onBrightness(onBrightness);
  myLight.onAdjustBrightness(onAdjustBrightness);
  myLight.onColor(onSetColor);

  // setup SinricPro
  SinricPro.onConnected([](){ DEBUG_PRINT("Connected to SinricPro" CLI_NL); }); 
  SinricPro.onDisconnected([](){ DEBUG_PRINT("Disconnected from SinricPro" CLI_NL);  NeoPixelShowStatusError();});
  SinricPro.begin(APP_KEY, APP_SECRET);

}

void SinricHandle(){
    SinricPro.handle();
}

#undef DEBUG_MSG