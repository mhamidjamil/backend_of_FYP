
#include <Ticker.h>
#include "Network.h"

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY !)
#error Select ESP32 board.
#endif

// Display *display;
Network *network;

void tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();

/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = NULL;
/** Ticker for temperature reading */
Ticker tempTicker;
/** Flag if task should run */
bool tasksEnabled = false;

bool initTemp()
{
  byte resultValue = 0;
  xTaskCreatePinnedToCore(
      tempTask,        /* Function to implement the task */
      "tempTask ",     /* Name of the task */
      10000,           /* Stack size in words */
      NULL,            /* Task input parameter */
      5,               /* Priority of the task */
      &tempTaskHandle, /* Task handle. */
      1);              /* Core where the task should run */

  if (tempTaskHandle == NULL)
  {
    Serial.println("Failed to start task for temperature update");
    return false;
  }
  else
  {
    // Start update of environment data every 20 seconds
    tempTicker.attach(20, triggerGetTemp);
  }
  return true;
}

void triggerGetTemp()
{
  if (tempTaskHandle != NULL)
  {
    xTaskResumeFromISR(tempTaskHandle);
  }
}

void tempTask(void *pvParameters)
{
  Serial.println("tempTask loop started");
  while (1) // tempTask loop
  {
    if (tasksEnabled)
    {
      getTemperature();
    }
    // Got sleep again
    vTaskSuspend(NULL);
  }
}
bool getTemperature()
{

  //    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
  network->firestoreDataUpdate((random(45, 50)+0.2), (random(45, 50)+0.3));

  return true;
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("DHT ESP32 example with tasks");

  //  initDisplay();
  initNetwork();
  initTemp();
  // Signal end of setup() to tasks
  tasksEnabled = true;
}

void loop()
{
  if (!tasksEnabled)
  {
    // Wait 2 seconds to let system settle down
    delay(2000);
    // Enable task that will read values from the DHT sensor
    tasksEnabled = true;
    if (tempTaskHandle != NULL)
    {
      vTaskResume(tempTaskHandle);
    }
  }
  yield();
}

void initNetwork()
{
  network = new Network();
  network->initWiFi();
}
