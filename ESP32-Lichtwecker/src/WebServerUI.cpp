#include "main.h"
#include "Debug.h"
#include "WebServerUI.h"
#include <DNSServer.h>
#include <ESPUI.h>
#include "IPAddress.h"
#include "myLED.h"

#define DEBUG_MSG DEBUG_MSG_WEBUI

const byte DNS_PORT = 53;
IPAddress apIP( 192, 168, 1, 1 );
DNSServer dnsServer;

void slider(Control *sender, int type);

void slider(Control *sender, int type) {
  DEBUG_PRINT("Slider ID: %d, Value: %d" CLI_NL,sender->id,sender->value.toInt());
  LedWakeFanSetDutyCycle(sender->value.toInt());
  LedWakeSetDutyCycle(sender->value.toInt());
}


void TaskWebUI(void *arg)
{
    DEBUG_PRINT("Created TaskWebUI" CLI_NL);
    //IPAddress apIP = WiFi.softAPIP();
    dnsServer.start(DNS_PORT, "*", apIP);

    ESPUI.addControl(ControlType::Slider, "Slider two", "100", ControlColor::Alizarin, Control::noParent, &slider);
    ESPUI.sliderContinuous = true;
    ESPUI.begin("ESPUI Control");

    while(1){
        dnsServer.processNextRequest();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

#undef DEBUG_MSG

