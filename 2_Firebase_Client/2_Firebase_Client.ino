
#include <Ticker.h>
#include "Network.h"

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY !)
#error Select ESP32 board.
#endif
double R_temperature;
// Display *display;
Network *network;
// ? data to be managed in struct
// b'25.18!Supine position!C:0.00,R:0.00,V:0.0000!0.00!'
int data_input = 0;

int filterd_temperatur = 0;

String position = "";

float filterd_Conductance = 0;
float filterd_Resistance = 0.0;
float filterd_Conductance_voltage = 0.0;

float filterd_snore_voltages=0.0;

    // ? data manager variables end
    void
    tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();
void DataManager(String tempstr);
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
  if (R_temperature > 10)
  {
    network->firestoreDataUpdate(R_temperature, (random(45, 50) + 0.3));
  }
  else
  {
    network->firestoreDataUpdate((random(55, 60) + 0.3), (random(45, 50) + 0.3));
  }

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
  // read data from serail terminal and assign that to double -> temperature

  String tempStr;
  if (Serial.available() > 0)
  {
    data_input = 1;
    while (Serial.available())
    {
      tempStr = Serial.readStringUntil('#');
    }

    Serial.println(tempStr);

    // R_temperature = Serial.parseFloat();
    // network->firestoreDataUpdate(temperature, 0);
  }

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

void DataManager(String tempstr)
{
  float filterd_temperatur = 0;

  String position = "";

  float filterd_Conductance = 0;
  float filterd_Resistance = 0.0;
  float filterd_Conductance_voltage = 0.0;

  float filterd_snore_voltages = 0.0;
  int first_exclamation = tempstr.indexOf('!');
  int second_exclamation = tempstr.indexOf('!', first_exclamation + 1);
  int third_exclamation = tempstr.indexOf('!', second_exclamation + 1);
  int fourth_exclamation = tempstr.indexOf('!', third_exclamation + 1);

  // Serial.println("1st ! sppoted at position : " + String(first_exclamation));
  // Serial.println("2nd ! sppoted at position : " + String(second_exclamation));
  // Serial.println("3rd ! sppoted at position : " + String(third_exclamation));

  String temp = tempstr.substring(2, first_exclamation);
  filterd_temperatur = temp.toFloat();

  Serial.println("filterd_temperatur: " + String(filterd_temperatur));

  position = tempstr.substring(first_exclamation + 1, second_exclamation);
  Serial.println("position: " + position);

  String temp1 = tempstr.substring(second_exclamation + 1, third_exclamation);
  int first_coma = temp1.indexOf(',');
  int second_coma = temp1.indexOf(',', first_coma + 1);
  int third_coma = temp1.indexOf(',', second_coma + 1);

  String temp2 = temp1.substring(0, first_coma);
  filterd_Conductance = temp2.toFloat();
  Serial.println("filterd_Conductance: " + String(filterd_Conductance));

  String temp3 = temp1.substring(first_coma + 1, second_coma);
  filterd_Resistance = temp3.toFloat();
  Serial.println("filterd_Resistance: " + String(filterd_Resistance));

  String temp4 = temp1.substring(second_coma + 1, third_exclamation);
  filterd_Conductance_voltage = temp4.toFloat();
  Serial.println("filterd_Conductance_voltage: " +
                 String(filterd_Conductance_voltage));

  String temp5 = tempstr.substring(third_exclamation + 1, fourth_exclamation);
  filterd_snore_voltages = temp5.toFloat();
  Serial.println("filterd_snore_voltages: " + String(filterd_snore_voltages));
}
