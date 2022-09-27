#include "main.h"
#include "CLI.h"
#include "SimpleCLI.h"
#include "WiFiServer.h"
//#include "Husarnet.h"

#define DEBUG_MSG DEBUG_MSG_CLI

void TaskCli(void *arg);
void CliClientDisconnectedCallback();
void CliClientConnectedCallback();

//HusarnetServer server(CLI_PORT);
SimpleCLI cli;
WiFiServer CliServer(8001);
WiFiClient client;

void CliInit(){

    DEBUG_PRINT("Init CLI" CLI_NL);

    xTaskCreate(
      TaskCli,  /* Task function. */
      "TaskCli",       /* String with name of task. */
      20000,            /* Stack size in bytes. */
      NULL,             /* Parameter passed as input of the task */
      2,                /* Priority of the task. */
      NULL);            /* Task handle. */

}

int CliWrite(const char *str){
    if(client.connected()){
        client.write(str);
    }
}

void CliClientConnectedCallback(){

}

void CliClientDisconnectedCallback(){

}

void TaskCli(void *arg){
  CliServer.begin();

  while (1) {
    client = CliServer.accept();
    if(client){
        CliClientConnectedCallback();
        while(client.connected()){
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        client.stop();
        CliClientDisconnectedCallback();
        client = NULL;
  
    }else{
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}
#undef DEBUG_MSG