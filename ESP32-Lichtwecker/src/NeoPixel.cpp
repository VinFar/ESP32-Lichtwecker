#include "NeoPixel.h"
#include <Adafruit_NeoPixel.h>
#include "Debug.h"
#include "ESPUI.h"

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
void setAll(byte red, byte green, byte blue);
void setPixel(int Pixel, byte red, byte green, byte blue);

static int RainbowCircleEffect(void *arg);
int FadeInFadeOut(void *arg);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

static const char *WebUiRGBTabName = "RGB Light";

uint16_t WebUiRGBTab;
uint16_t WebUiRGBEffectSwitchID;
uint16_t WebUiRGBEffectSelector;

uint16_t WebUiRGBSliderRed;
uint16_t WebUiRGBSliderGreen;
uint16_t WebUiRGBSliderBlue;
uint16_t WebUiRGBSliderBrightness;

uint8_t NeoPixelCurrentRed;
uint8_t NeoPixelCurrentGreen;
uint8_t NeoPixelCurrentBlue;
uint8_t NeoPixelCurrentBrightness;

int RGBEffectStatus = false;
static bool ClearedEffect = false;
static bool UpdateLeds = true;
int (*CurrentEffectFunction)(void *arg) = RainbowCircleEffect;
TaskHandle_t TaskNeoPixelTaskHandle;

typedef struct
{
    const char *EffectName;
    int (*CallBack)(void *arg);
} RgbEffect_t;

const RgbEffect_t RgbEffects[] = {{"Rainbow", RainbowCircleEffect},
                                  {"FadeIn/FadeOut", FadeInFadeOut}};

void NeoPixelInit()
{

    strip.begin();
    strip.setBrightness(127);
    strip.clear();
    strip.show();
    DEBUG_PRINT("Init NeoPixels" CLI_NL);

    CurrentEffectFunction = RainbowCircleEffect;
}

void WebUiRGBInit()
{

    WebUiRGBEffectSwitchID = ESPUI.addControl(ControlType::Switcher, "Effekt Ein/aus", "", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBSwitcherCallback);

    WebUiRGBEffectSelector = ESPUI.addControl(ControlType::Select, "Effekt auswählen", "", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBEffectSelect);

    for (int i = 0; i < ARRAY_LEN(RgbEffects); i++)
    {
        ESPUI.addControl(ControlType::Option, RgbEffects[i].EffectName, RgbEffects[i].EffectName, ControlColor::Alizarin, WebUiRGBEffectSelector);
    }

    WebUiRGBSliderRed = ESPUI.addControl(ControlType::Slider, "Rot", "0", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBSliderCallback);

    WebUiRGBSliderGreen = ESPUI.addControl(ControlType::Slider, "Gruen", "0", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBSliderCallback);

    WebUiRGBSliderBlue = ESPUI.addControl(ControlType::Slider, "Blau", "0", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBSliderCallback);

    WebUiRGBSliderBrightness = ESPUI.addControl(ControlType::Slider, "Helligkeit", "0", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBSliderCallback);
}

void WebUiRGBCreate()
{
    WebUiRGBTab = ESPUI.addControl(ControlType::Tab, WebUiRGBTabName, WebUiRGBTabName);
    DEBUG_PRINT("Created RGB Tab" CLI_NL);
}

static void WebUiRGBEffectSelect(Control *Switcher, int value)
{

    DEBUG_PRINT("Selected %s" CLI_NL, Switcher->value.c_str());
    int IdxForEffectArray = RgbEffectGetIndex(Switcher->value.c_str());
    CurrentEffectFunction = RgbEffects[IdxForEffectArray].CallBack;
    vTaskDelete(TaskNeoPixelTaskHandle);
    TaskNeoPixelStart();
}

static int RgbEffectGetIndex(const char *Effect)
{

    for (int i = 0; i < ARRAY_LEN(RgbEffects); i++)
    {
        if (!strcmp(RgbEffects[i].EffectName, Effect))
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

static void WebUiRGBSliderCallback(Control *Slider, int value)
{

    float valueFromSlider = Slider->value.toFloat() * 2.55f;
    // DEBUG_PRINT("Value from Silder: %f" CLI_NL, valueFromSlider);
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
        strip.setBrightness(NeoPixelCurrentBrightness);
        UpdateLeds = true;
        return;
    }
    else
    {
        return;
    }
    uint32_t color = strip.Color(NeoPixelCurrentRed, NeoPixelCurrentGreen, NeoPixelCurrentBlue);
    strip.fill(color);
    UpdateLeds = true;
}

static int RainbowCircleEffect(void *arg)
{
    static int position = 0;
    for (int i = 0; i < CNT; i++)
        strip.setPixelColor((i + position) % CNT, getPixelColorHsv(i, i * (MAXHUE / CNT), 255, 100));
    position++;
    position %= CNT;
    strip.show();
    vTaskDelay(pdMS_TO_TICKS(100));
}

int FadeInFadeOut(void *arg)
{
    for (int j = 0; j < 3; j++)
    {
        // Fade IN
        for (int k = 0; k < 256; k++)
        {
            switch (j)
            {
            case 0:
                setAll(k, 0, 0);
                break;
            case 1:
                setAll(0, k, 0);
                break;
            case 2:
                setAll(0, 0, k);
                break;
            }
            showStrip();
            vTaskDelay(pdMS_TO_TICKS(3));
        }
        // Fade OUT
        for (int k = 255; k >= 0; k--)
        {
            switch (j)
            {
            case 0:
                setAll(k, 0, 0);
                break;
            case 1:
                setAll(0, k, 0);
                break;
            case 2:
                setAll(0, 0, k);
                break;
            }
            showStrip();
            vTaskDelay(pdMS_TO_TICKS(3));
        }
    }
    return 0;
}

void TaskNeoPixel(void *arg)
{
    DEBUG_PRINT("Created Task Neo Pixel" CLI_NL);
    TaskNeoPixelTaskHandle = xTaskGetCurrentTaskHandle();
    while (1)
    {
        if (RGBEffectStatus)
        {
            CurrentEffectFunction(NULL);
        }
        else
        {
            if (!ClearedEffect)
            {
                strip.clear();
                strip.show();
                ClearedEffect = true;
            }
            if (UpdateLeds)
            {
                UpdateLeds = false;
                strip.show();
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
        // vTaskSuspendAll();
        // xTaskResumeAll();
    }
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

void showStrip()
{
    strip.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue)
{
    strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue)
{
    for (int i = 0; i < CNT; i++)
    {
        setPixel(i, red, green, blue);
    }
    showStrip();
}

void TaskNeoPixelStart()
{
    xTaskCreatePinnedToCore(TaskNeoPixel, "NeoPixelTask", 4000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

#undef DEBUG_MSG