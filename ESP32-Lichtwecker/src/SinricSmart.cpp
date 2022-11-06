#include "SinricPro.h"
#include "SinricProLight.h"
#include "Debug.h"
#include "myLED.h"
#include "SinricCredentials.h"
#include "NeoPixel.h"
<<<<<<< HEAD

#define DEBUG_MSG DEBUG_MSG_NEOPIXEL

=======

#if defined(APP_KEY) || defined(APP_SECRET) || defined(LIGHT_ID)
#else
#error "You need to define your Sinric Credentials"
#endif

#define DEBUG_MSG DEBUG_MSG_NEOPIXEL

SinricProLight &myLight = SinricPro[LIGHT_ID];
>>>>>>> 8f376aa05ba98f37348fb990b5e0a66ab842ea6d

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
<<<<<<< HEAD
    setAll(0,0,0);
=======
    NeoPixelAllOff();
>>>>>>> 8f376aa05ba98f37348fb990b5e0a66ab842ea6d
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

<<<<<<< HEAD
bool onColor(const String &deviceId, byte &r, byte &g, byte &b) {
  DEBUG_PRINT("Color set to: %d Red, %d Green, %d Blue" CLI_NL,r,g,b);
  setAll(r,g,b);
  return true;
}

=======
bool onSetColor(const String &deviceId, byte &R, byte &G, byte &B){
  DEBUG_PRINT("Device Color set to: R:%d, G:%d, B:%d" CLI_NL,R,G,B);
  NeoPixelsetAll(R,G,B);
  return true;
}

void SinricSendCurrentBrightness(int Brightness){
  myLight.sendBrightnessEvent(Brightness);
}

>>>>>>> 8f376aa05ba98f37348fb990b5e0a66ab842ea6d
void setupSinricPro() {
  // get a new Light device from SinricPro
  

  
  // set callback function to device
  myLight.onPowerState(onPowerState);
  myLight.onBrightness(onBrightness);
  myLight.onAdjustBrightness(onAdjustBrightness);
<<<<<<< HEAD
  myLight.onColor(onColor);
=======
  myLight.onColor(onSetColor);
>>>>>>> 8f376aa05ba98f37348fb990b5e0a66ab842ea6d

  // setup SinricPro
  SinricPro.onConnected([](){ DEBUG_PRINT("Connected to SinricPro" CLI_NL); }); 
  SinricPro.onDisconnected([](){ DEBUG_PRINT("Disconnected from SinricPro" CLI_NL);  NeoPixelShowStatusError();});
  SinricPro.begin(APP_KEY, APP_SECRET);

}

void SinricHandle(){
    SinricPro.handle();
}

#undef DEBUG_MSG