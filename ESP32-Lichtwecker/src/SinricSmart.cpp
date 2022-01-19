#include "SinricPro.h"
#include "SinricProLight.h"
#include "Debug.h"
#include "myLED.h"

#define DEBUG_MSG DEBUG_MSG_NEOPIXEL

#define APP_KEY           "3587cd20-08b9-4e96-baa1-72c7588fa45e"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "5524df8c-a47b-41f4-9237-7aef42f2aa9d-89df2f43-935e-4f90-90cf-fbce97b6dee9"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define LIGHT_ID          "61dc99af7c2a5a7bcddafcf7"    // Should look like "5dc1564130xxxxxxxxxxxxxx"

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

void setupSinricPro() {
  // get a new Light device from SinricPro
  SinricProLight &myLight = SinricPro[LIGHT_ID];

  
  // set callback function to device
  myLight.onPowerState(onPowerState);
  myLight.onBrightness(onBrightness);
  myLight.onAdjustBrightness(onAdjustBrightness);

  // setup SinricPro
  SinricPro.onConnected([](){ DEBUG_PRINT("Connected to SinricPro" CLI_NL); }); 
  SinricPro.onDisconnected([](){ DEBUG_PRINT("Disconnected from SinricPro" CLI_NL); });
  SinricPro.begin(APP_KEY, APP_SECRET);

}

void SinricHandle(){
    SinricPro.handle();
}

#undef DEBUG_MSG