#include "Button.h"
#include "OneButton.h"
#include "main.h"
#include "Debug.h"

#define DEBUG_MSG DEBUG_MSG_BUTTON

OneButton button(BUTTON_PIN, true);

void ButtonDoubleClickCallback();


void ButtonInit(){

  pinMode(BUTTON_PIN, INPUT); 

    button.attachDoubleClick(ButtonDoubleClickCallback);
    button.attachClick(ButtonSingleClick);
}

void ButtonTick(){
    button.tick();
}

void ButtonSingleClick(){
    DEBUG_PRINT("Button Single Click" CLI_NL);
}

void ButtonDoubleClickCallback(){
    DEBUG_PRINT("Button Double Click" CLI_NL);
}

#undef DEBUG_MSG