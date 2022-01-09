#include "NeoPixel.h"
#include <Adafruit_NeoPixel.h>
#include "Debug.h"
#include "ESPUI.h"

#define DEBUG_MSG DEBUG_MSG_NEOPIXEL
#define CNT 24
#define MAXHUE 256 * 6

uint32_t getPixelColorHsv(
    uint16_t n, uint16_t h, uint8_t s, uint8_t v);
void TaskNeoPixel(void *arg);
static void WebUiRGBSwitcherCallback(Control *Switcher, int value);
static void WebUiRGBSliderCallback(Control *Slider, int value);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

static const char *WebUiRGBTabName = "RGB Light";

uint16_t WebUiRGBTab;
uint16_t WebUiRGBEffectSwitchID;

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
static bool UpdateLeds=true;

void NeoPixelInit()
{

    strip.begin();
    strip.setBrightness(100);
    strip.clear();
    strip.show();
    DEBUG_PRINT("Init NeoPixels" CLI_NL);
    xTaskCreatePinnedToCore(TaskNeoPixel, "NeoPixelTask", 8000, NULL, 4, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void WebUiRGBInit()
{

    WebUiRGBEffectSwitchID = ESPUI.addControl(ControlType::Switcher, "Effekt Ein/aus", "", ControlColor::Alizarin, WebUiRGBTab, &WebUiRGBSwitcherCallback);

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
    //DEBUG_PRINT("Value from Silder: %f" CLI_NL, valueFromSlider);
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
    }
    else
    {
        return;
    }
    uint32_t color = strip.Color(NeoPixelCurrentRed, NeoPixelCurrentGreen, NeoPixelCurrentBlue);
    strip.fill(color);
    strip.setBrightness(NeoPixelCurrentBrightness);
    UpdateLeds=true;
}

void TaskNeoPixel(void *arg)
{
    int position = 0;
    DEBUG_PRINT("Created Task Neo Pixel" CLI_NL);

    while (1)
    {
        if (RGBEffectStatus)
        {
            for (int i = 0; i < CNT; i++)
                strip.setPixelColor((i + position) % CNT, getPixelColorHsv(i, i * (MAXHUE / CNT), 255, 100));
            position++;
            position %= CNT;
            strip.show();
        }
        else
        {
            if (!ClearedEffect)
            {
                strip.clear();
                strip.show();
                ClearedEffect = true;
            }
            if(UpdateLeds){
                UpdateLeds=false;
                strip.show();
            }
        }
        // vTaskSuspendAll();
        // xTaskResumeAll();
        vTaskDelay(pdMS_TO_TICKS(50));
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

#undef DEBUG_MSG