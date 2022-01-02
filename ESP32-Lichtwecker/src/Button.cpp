#include "Button.h"
#include "OneButton.h"
#include "main.h"
#include "Debug.h"
#include "AlarmTask.h"

#define DEBUG_MSG DEBUG_MSG_BUTTON

OneButton button(BUTTON_PIN, true);

static void ButtonDoubleClickCallback();
static void ButtonSingleClick();

void ButtonInit(){

  pinMode(BUTTON_PIN, INPUT); 

    button.attachDoubleClick(ButtonDoubleClickCallback);
    button.attachClick(ButtonSingleClick);
}

void ButtonTick(){
    button.tick();
}

static void ButtonSingleClick(){
    DEBUG_PRINT("Button Single Click" CLI_NL);
    if(ButtonSingleClickAlarmCallback())
        return;
}

static void ButtonDoubleClickCallback(){
    DEBUG_PRINT("Button Double Click" CLI_NL);
}

#undef DEBUG_MSG