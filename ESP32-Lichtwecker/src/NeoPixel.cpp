#include "NeoPixel.h"
#include <Adafruit_NeoPixel.h>
#include "Debug.h"
#include "ESPUI.h"
#include <WS2812FX.h>

#define DEBUG_MSG DEBUG_MSG_NEOPIXEL
#define CNT 24
#define MAXHUE 256 * 6

uint32_t getPixelColorHsv(
    uint16_t n, uint16_t h, uint8_t s, uint8_t v);
static void WebUiRGBSwitcherCallback(Control *Switcher, int value);
static void WebUiRGBSliderCallback(Control *Slider, int value);
static void WebUiRGBEffectSelect(Control *Switcher, int value);
static int RgbEffectGetIndex(const char *Effect);
void showStrip();

void setPixel(int Pixel, byte red, byte green, byte blue);
void WebUiSliderSpeedCallback(Control *Slider, int value);
void NeoPixelCallbackFromTimer(TimerHandle_t xTimer);

WS2812FX ws2812fx = WS2812FX(CNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

static const char *WebUiRGBTabName = "RGB Light";

uint16_t WebUiRGBTab;
uint16_t WebUiRGBEffectSwitchID;
uint16_t WebUiRGBEffectSelector;

uint16_t WebUiRGBSliderRed;
uint16_t WebUiRGBSliderGreen;
uint16_t WebUiRGBSliderBlue;
uint16_t WebUiRGBSliderBrightness;
uint16_t WebUiRGBSliderSpeed;

uint8_t NeoPixelCurrentRed;
uint8_t NeoPixelCurrentGreen;
uint8_t NeoPixelCurrentBlue;
uint8_t NeoPixelCurrentBrightness;

int RGBEffectStatus = false;
static bool ClearedEffect = true;
static bool UpdateLeds = true;
float RGBEffectCurrentDelay = 1.0;

void NeoPixelInit()
{
    DEBUG_PRINT("Init NeoPixels" CLI_NL);

    ws2812fx.init();
    ws2812fx.setBrightness(100);
    ws2812fx.setSpeed(2200);
    ws2812fx.clear();
    ws2812fx.start();
    NeoPixelShowStatusBooting();
    RGBEffectStatus=true;

}

void NeoPixelSetColorForAnimation(uint8_t R, uint8_t G, uint8_t B){
    if(!RGBEffectStatus)
        ws2812fx.setMode(FX_MODE_FADE);
    ws2812fx.setColor(ws2812fx.Color(R,G,B));
}

void NeoPixelShowStatusError(){
    NeoPixelSetColorForAnimation(255,0,0);
}

void NeoPixelShowStatusBooting(){
    NeoPixelSetColorForAnimation(0,0,255);
}

void NeoPixelShowStatusWifiConfig(){
    NeoPixelSetColorForAnimation(255,165,0);
}

void NeoPixelShowStatusBootOk(){
    NeoPixelSetColorForAnimation(0,255,0);
    TimerHandle_t Timer =  xTimerCreate("",pdMS_TO_TICKS(7000),pdFALSE,(void*)0,NeoPixelCallbackFromTimer);
    xTimerStart(Timer,pdMS_TO_TICKS(100));
}

void WebUiRGBInit()
{

    WebUiRGBEffectSwitchID = ESPUI.addControl(ControlType::Switcher, "Effekt Ein/aus", "", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBSwitcherCallback);

    WebUiRGBEffectSelector = ESPUI.addControl(ControlType::Select, "Effekt auswählen", "", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBEffectSelect);

    for (int i = 0; i < ws2812fx.getModeCount() - 8; i++)
    {
        ESPUI.addControl(ControlType::Option, (const char *)ws2812fx.getModeName(i), (const char *)ws2812fx.getModeName(i), ControlColor::Alizarin, WebUiRGBEffectSelector);
    }

    WebUiRGBSliderSpeed = ESPUI.addControl(ControlType::Slider, "Geschwindigkeit", "50", ControlColor::Alizarin, WebUiRGBTab, &WebUiSliderSpeedCallback);

    WebUiRGBSliderRed = ESPUI.addControl(ControlType::Slider, "Rot", "0", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBSliderCallback);

    WebUiRGBSliderGreen = ESPUI.addControl(ControlType::Slider, "Gruen", "0", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBSliderCallback);

    WebUiRGBSliderBlue = ESPUI.addControl(ControlType::Slider, "Blau", "0", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBSliderCallback);

    WebUiRGBSliderBrightness = ESPUI.addControl(ControlType::Slider, "Helligkeit", "0", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBSliderCallback);
}

void WebUiSliderSpeedCallback(Control *Slider, int value)
{
    RGBEffectCurrentDelay = -Slider->value.toFloat() * 0.018 + 1.9;
    ws2812fx.setSpeed((int)(5000.0f * RGBEffectCurrentDelay));
    DEBUG_PRINT("Geschw. Mult.: %.3f" CLI_NL, RGBEffectCurrentDelay);
}

void WebUiRGBCreate()
{
    WebUiRGBTab = ESPUI.addControl(ControlType::Tab, WebUiRGBTabName, WebUiRGBTabName);
    DEBUG_PRINT("Created RGB Tab" CLI_NL);
}

static void WebUiRGBEffectSelect(Control *Switcher, int value)
{
    int IdxForEffectArray = RgbEffectGetIndex(Switcher->value.c_str());
    ws2812fx.setMode(IdxForEffectArray);
    DEBUG_PRINT("Selected %s. IDX %d" CLI_NL, Switcher->value.c_str(),IdxForEffectArray);
}

static int RgbEffectGetIndex(const char *Effect)
{

    for (int i = 0; i < ws2812fx.getModeCount() - 8; i++)
    {
        if (!strcmp((const char *)ws2812fx.getModeName(i), Effect))
        {
            return i;
        }
    }
    return 0;
}

static void WebUiRGBSwitcherCallback(Control *Switcher, int value)
{
    int valueFromSwitcher = Switcher->value.toInt();
    DEBUG_PRINT("RGB Effect %s" CLI_NL, valueFromSwitcher ? "on" : "off");
    RGBEffectStatus = valueFromSwitcher;
    ClearedEffect = false;
}

void NeoPixelAllOff(){
    RGBEffectStatus=false;
    ClearedEffect=false;
}

void NeoPixelBlinkForFeedback(uint8_t R, uint8_t G, uint8_t B){
    NeoPixelsetAll(R,G,B);
    TimerHandle_t Timer =  xTimerCreate("",pdMS_TO_TICKS(100),pdFALSE,(void*)0,NeoPixelCallbackFromTimer);
    xTimerStart(Timer,pdMS_TO_TICKS(100));
}

void NeoPixelCallbackFromTimer(TimerHandle_t xTimer){
    NeoPixelAllOff();
}


static void WebUiRGBSliderCallback(Control *Slider, int value)
{

    float valueFromSlider = Slider->value.toFloat() * 2.55f;
    int ID = Slider->id;

    if (valueFromSlider > 255.0f)
    {
        valueFromSlider = 255.0f;
    }
    if (valueFromSlider < 0.0f)
    {
        valueFromSlider = 0.0f;
    }

    if (ID == WebUiRGBSliderRed)
    {
        NeoPixelCurrentRed = (uint8_t)(valueFromSlider);
        DEBUG_PRINT("NeoPixel Red set to %d" CLI_NL, NeoPixelCurrentRed);
    }
    else if (ID == WebUiRGBSliderGreen)
    {
        NeoPixelCurrentGreen = (uint8_t)(valueFromSlider);
        DEBUG_PRINT("NeoPixel Red set to %d" CLI_NL, NeoPixelCurrentGreen);
    }
    else if (ID == WebUiRGBSliderBlue)
    {
        NeoPixelCurrentBlue = (uint8_t)(valueFromSlider);
        DEBUG_PRINT("NeoPixel Red set to %d" CLI_NL, NeoPixelCurrentBlue);
    }
    else if (ID == WebUiRGBSliderBrightness)
    {
        NeoPixelCurrentBrightness = (uint8_t)(valueFromSlider);
        DEBUG_PRINT("NeoPixel Brightness set to %d" CLI_NL, WebUiRGBSliderBrightness);
        ws2812fx.setBrightness(NeoPixelCurrentBrightness);
        UpdateLeds = true;
        return;
    }
    else
    {
        return;
    }
    uint32_t color = ws2812fx.Color(NeoPixelCurrentRed, NeoPixelCurrentGreen, NeoPixelCurrentBlue);
    ws2812fx.fill(color);
    ws2812fx.setColor(color);
    UpdateLeds = true;
}

void showStrip()
{
    ws2812fx.show();
}

uint32_t getPixelColorHsv(
    uint16_t n, uint16_t h, uint8_t s, uint8_t v)
{

    uint8_t r, g, b;

    if (!s)
    {
        // Monochromatic, all components are V
        r = g = b = v;
    }
    else
    {
        uint8_t sextant = h >> 8;
        if (sextant > 5)
            sextant = 5; // Limit hue sextants to defined space

        g = v; // Top level

        // Perform actual calculations

        /*
           Bottom level:
           --> (v * (255 - s) + error_corr + 1) / 256
        */
        uint16_t ww; // Intermediate result
        ww = v * (uint8_t)(~s);
        ww += 1;       // Error correction
        ww += ww >> 8; // Error correction
        b = ww >> 8;

        uint8_t h_fraction = h & 0xff; // Position within sextant
        uint32_t d;                    // Intermediate result

        if (!(sextant & 1))
        {
            // r = ...slope_up...
            // --> r = (v * ((255 << 8) - s * (256 - h)) + error_corr1 + error_corr2) / 65536
            d = v * (uint32_t)(0xff00 - (uint16_t)(s * (256 - h_fraction)));
            d += d >> 8; // Error correction
            d += v;      // Error correction
            r = d >> 16;
        }
        else
        {
            // r = ...slope_down...
            // --> r = (v * ((255 << 8) - s * h) + error_corr1 + error_corr2) / 65536
            d = v * (uint32_t)(0xff00 - (uint16_t)(s * h_fraction));
            d += d >> 8; // Error correction
            d += v;      // Error correction
            r = d >> 16;
        }

        // Swap RGB values according to sextant. This is done in reverse order with
        // respect to the original because the swaps are done after the
        // assignments.
        if (!(sextant & 6))
        {
            if (!(sextant & 1))
            {
                uint8_t tmp = r;
                r = g;
                g = tmp;
            }
        }
        else
        {
            if (sextant & 1)
            {
                uint8_t tmp = r;
                r = g;
                g = tmp;
            }
        }
        if (sextant & 4)
        {
            uint8_t tmp = g;
            g = b;
            b = tmp;
        }
        if (sextant & 2)
        {
            uint8_t tmp = r;
            r = b;
            b = tmp;
        }
    }
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

void NeoPixelsetAll(byte red, byte green, byte blue)
{
     uint32_t color = ws2812fx.Color(red, green, blue);
     ws2812fx.fill(color);
     //ws2812fx.setColor(color);
     UpdateLeds=true;
}

void NeoPixelSetBrightness(byte brightness){
    ws2812fx.setBrightness(brightness);
    UpdateLeds = true;
}

void NeoPixelTick()
{

    if (RGBEffectStatus)
    {
        ws2812fx.service();
        // CurrentEffectFunction(NULL);
    }
    else
    {
        if (!ClearedEffect)
        {
            ws2812fx.clear();
            ws2812fx.show();
            ClearedEffect = true;
        }
        if (UpdateLeds)
        {
            UpdateLeds = false;
            ws2812fx.show();
        }
    }
}

int ButtonNeoPixelSingleClickCallback()
{

    if (RGBEffectStatus)
    {
        RGBEffectStatus = false;
        DEBUG_PRINT("RGB Effect off over Button" CLI_NL);
        return 1;
    }
    ClearedEffect=false;
    return 0;
}

#undef DEBUG_MSG