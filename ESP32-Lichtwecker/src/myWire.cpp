#include "main.h"
#include "myWire.h"
#include "Debug.h"

#define DEBUG_MSG DEBUG_MSG_MYWIRE

SemaphoreHandle_t myWireSemaphore = NULL;
StaticSemaphore_t myWireSemaphoreBuffer;

 void myWireInit(){

    DEBUG_PRINT("Creating Semaphore" CLI_NL);
    myWireSemaphore = xSemaphoreCreateBinaryStatic( &myWireSemaphoreBuffer );

    configASSERT(myWireSemaphore);

    myWireSemaphoreGive();

}

UBaseType_t myWireSemaphoreTake(TickType_t TimeToWait){
    //DEBUG_PRINT("Taking Semaphore" CLI_NL);
    return xSemaphoreTake(myWireSemaphore, TimeToWait);
}

UBaseType_t myWireSemaphoreGive(){
    //DEBUG_PRINT("Releasing Semaphore" CLI_NL);
    return xSemaphoreGive(myWireSemaphore);
}

#undef DEBUG_MSG
 