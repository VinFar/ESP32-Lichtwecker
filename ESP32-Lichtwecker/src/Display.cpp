#include "Display.h"
#include "myWire.h"

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
        display.display();
        vTaskDelay(pdMS_TO_TICKS(2000));

        display.clearDisplay();

        display.setTextSize(2); // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(10, 0);
        display.println(F("scroll"));
        display.display(); // Show initial text
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
            display.display();
    }
}


#undef DEBUG_MSG