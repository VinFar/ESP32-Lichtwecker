#include "Display.h"
#include "myWire.h"
#include "myWifi.h"
#include "IPAddress.h"
#include "SinricSmart.h"
#include "TemperatureSensor.h"

#define DEBUG_MSG DEBUG_MSG_DISPLAY

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define XPOS 0 // Indexes into the 'icons' array in function below
#define YPOS 1
#define DELTAY 2
#define NUMFLAKES 10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16
static const unsigned char PROGMEM logo_bmp[] =
    {0b00000000, 0b11000000,
     0b00000001, 0b11000000,
     0b00000001, 0b11000000,
     0b00000011, 0b11100000,
     0b11110011, 0b11100000,
     0b11111110, 0b11111000,
     0b01111110, 0b11111111,
     0b00110011, 0b10011111,
     0b00011111, 0b11111100,
     0b00001101, 0b01110000,
     0b00011011, 0b10100000,
     0b00111111, 0b11100000,
     0b00111111, 0b11110000,
     0b01111100, 0b11110000,
     0b01110000, 0b01110000,
     0b00000000, 0b00110000};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

static uint32_t _LastMillis;
static uint32_t _MillisInterval = 857;

void DisplayInit()
{

    if(myWireSemaphoreTake(pdMS_TO_TICKS(3000)) == pdTRUE){
        if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
        {
            DEBUG_PRINT("SSD1306 allocation failed" CLI_NL);
        }
        else
        {
            DEBUG_PRINT("SSD1306 Init OK" CLI_NL);
        }
        DEBUG_PRINT("HGere1" CLI_NL);
        display.setRotation(2);
        display.clearDisplay();

        display.setTextSize(1); // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        myWireSemaphoreGive();
    }else{
        DEBUG_PRINT("Could not acquire Semaphore" CLI_NL);
    }
_LastMillis=millis();
}

void DisplayTick()
{
    if ((_LastMillis + _MillisInterval) < millis())
    {
        _LastMillis = millis();
        DisplaySetStatus(SinricStatus(), WifiGetStatus(), TempSensorStatus());
        display.display();
    }
}

void DisplaySetIPAddress(IPAddress ip){

    int16_t x1, y1;
    uint16_t w, h;
    String IPString = "IP: " + ip.toString();

    display.getTextBounds(IPString, 0, 0, &x1, &y1, &w, &h);

    w = display.width();

    DEBUG_PRINT("Setting IP Address on Display: %d.%d.%d.%d" CLI_NL,ip[0],ip[1],ip[2],ip[3]);

    display.setCursor(0,0);
    display.fillRect(x1, y1, w, h, BLACK);
    display.print(IPString);

}

void DisplaySetTemperature(float Temperature){

    int16_t x1;
    int16_t y1;
    uint16_t w;
    uint16_t h;
    String TempString = "Temp: " + String(Temperature, 2) + " C";

    display.getTextBounds(TempString, 0, 10, &x1, &y1, &w, &h);

    w = display.width();

    display.fillRect(x1, y1, w, h, BLACK);
    display.setCursor(0,10);
    display.print(TempString);
}

void DisplaySetPower(float Power){

    int16_t x1;
    int16_t y1;
    uint16_t w;
    uint16_t h;
    String TempString = "Power: " + String(Power, 2) + " %";

    display.getTextBounds(TempString, 0, 20, &x1, &y1, &w, &h);

    w = display.width();

    display.fillRect(x1, y1, w, h, BLACK);
    display.setCursor(0,20);
    display.print(TempString);
}

void DisplaySetStatus(uint8_t Sinric, uint8_t Wifi, uint8_t Temp){

    int16_t x1;
    int16_t y1;
    uint16_t w;
    uint16_t h;
    String WifiString = "NOK";
    String TempString = "NOK";
    String SinricString = "NOK";

    if(Wifi){
        WifiString = "OK";
    }

    if(Sinric){
        SinricString = "OK";
    }

    if(Temp){
        TempString = "OK";
    }

    String String1 = "Wifi: " + WifiString + " | Temp: " + TempString;
    String String2 = "Sinric: " + SinricString;

    display.getTextBounds(String1, 0, 30, &x1, &y1, &w, &h);

    w = display.width();

    display.fillRect(x1, y1, w, h, BLACK);
    display.setCursor(0,30);
    display.print(String1);

    display.getTextBounds(String2, 0, 40, &x1, &y1, &w, &h);

    w = display.width();

    display.fillRect(x1, y1, w, h, BLACK);
    display.setCursor(0,40);
    display.print(String2);

}


#undef DEBUG_MSG