
#include "Network.h"
#include <Ticker.h>

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY !)
#error Select ESP32 board.
#endif
// Display *display;
Network *network;
// ? data to be managed in struct
// b'25.18!Supine position!C:0.00,R:0.00,V:0.0000!0.00!'
// b'25.15!non-defined position!1.23,4.56,7.8912!3.56!#'
bool data_input = false;

String userID = "";

float filterd_temperatur = 0;

String position = "";

float filterd_Conductance = 0;
float filterd_Resistance = 0.0;
float filterd_Conductance_voltage = 0.0;

float filterd_snore_voltages = 0.0;

// ? data manager variables end
// 18 -> Red , 23 -> Green , 19 -> Blue
// #define RED 18
// #define GREEN 23
// #define BLUE 19

void tempTask(void *pvParameters);
bool Update_values();
void triggerGetTemp();
void DataManager(String tempstr);
/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = NULL;
/** Ticker for temperature reading */
Ticker tempTicker;
/** Flag if task should run */
bool tasksEnabled = false;

bool initTemp() {
  byte resultValue = 0;
  xTaskCreatePinnedToCore(tempTask,        /* Function to implement the task */
                          "tempTask ",     /* Name of the task */
                          10000,           /* Stack size in words */
                          NULL,            /* Task input parameter */
                          5,               /* Priority of the task */
                          &tempTaskHandle, /* Task handle. */
                          1);              /* Core where the task should run */

  if (tempTaskHandle == NULL) {
    Serial.println("Failed to start task for temperature update");
    return false;
  } else {
    // Start update of environment data every 20 seconds
    tempTicker.attach(20, triggerGetTemp);
  }
  return true;
}

void triggerGetTemp() {
  if (tempTaskHandle != NULL) {
    xTaskResumeFromISR(tempTaskHandle);
  }
}

void tempTask(void *pvParameters) {
  Serial.println("tempTask loop started");
  while (1) // tempTask loop
  {
    if (tasksEnabled) {
      Update_values();
    }
    // Got sleep again
    vTaskSuspend(NULL);
  }
}
bool Update_values() {

  //    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
  if (userID != "" && data_input) {
    network->firestoreDataUpdate(userID, filterd_temperatur, position,
                                 filterd_Conductance, filterd_Resistance,
                                 filterd_Conductance_voltage,
                                 filterd_snore_voltages);
    data_input = false;
  } else if (data_input) {
    network->firestoreDataUpdate(
        filterd_temperatur, position, filterd_Conductance, filterd_Resistance,
        filterd_Conductance_voltage, filterd_snore_voltages);
    data_input = false;
  }
  // else if (filterd_temperatur > 10)
  // {
  //   network->firestoreDataUpdate(filterd_temperatur, (random(45, 50) + 0.3));
  // }
  // else
  // {
  //   network->firestoreDataUpdate(37.4,
  //   unknown_state, 1.23, 2.34, 3.45, 4.56);
  // }
  //    else
  //  {
  //    network->firestoreDataUpdateResistance_GSR_(372113.62);
  //  }

  return true;
}

void setup() {
  Serial.begin(115200);
  // pinMode(RED, OUTPUT);
  // pinMode(GREEN, OUTPUT);
  // pinMode(BLUE, OUTPUT);
  Serial.println();
  Serial.println("DHT ESP32 example with tasks");

  //  initDisplay();
  initNetwork();
  initTemp();
  // Signal end of setup() to tasks
  tasksEnabled = true;
}

void loop() {
  // read data from serail terminal and assign that to double -> temperature

  String tempStr;
  if (Serial.available() > 0) {
    data_input = true;
    while (Serial.available()) {
      tempStr = Serial.readStringUntil('#');
    }
    if (tempStr.length() > 10)
      DataManager(tempStr);
    // Serial.println(tempStr);

    // filterd_temperatur = Serial.parseFloat();
    // network->firestoreDataUpdate(temperature, 0);
  }

  if (!tasksEnabled) {
    // Wait 2 seconds to let system settle down
    delay(2000);
    // Enable task that will read values from the DHT sensor
    tasksEnabled = true;
    if (tempTaskHandle != NULL) {
      vTaskResume(tempTaskHandle);
    }
  }
  yield();
}

void initNetwork() {
  network = new Network();
  network->initWiFi();
}

void DataManager(String tempstr) {
  Serial.println("Working on : " + tempstr);
  String user_id = "";

  float func_temperature = 0;

  String func_position = "";

  float func_Conductance = 0;
  float func_Resistance = 0.0;
  float func_Resistance_voltage = 0.0;
  float func_snore_voltages = 0.0;
  int first_at = tempstr.indexOf('@');
  int first_exclamation = tempstr.indexOf('!');
  int second_exclamation = tempstr.indexOf('!', first_exclamation + 1);
  int third_exclamation = tempstr.indexOf('!', second_exclamation + 1);
  int fourth_exclamation = tempstr.indexOf('!', third_exclamation + 1);

  // Serial.println("1st ! sppoted at position : " + String(first_exclamation));
  // Serial.println("2nd ! sppoted at position : " +
  // String(second_exclamation)); Serial.println("3rd ! sppoted at position : "
  // + String(third_exclamation));

  // String temp = tempstr.substring(2, first_exclamation);
  user_id = tempstr.substring(0, first_at);
  Serial.println("user_id: " + user_id);

  String temp = tempstr.substring(first_at + 1, first_exclamation);
  func_temperature = temp.toFloat();

  Serial.println("func_temperature: " + String(func_temperature));

  func_position = tempstr.substring(first_exclamation + 1, second_exclamation);
  Serial.println("position: " + func_position);

  String temp1 = tempstr.substring(second_exclamation + 1, third_exclamation);
  int first_coma = temp1.indexOf(',');
  int second_coma = temp1.indexOf(',', first_coma + 1);
  int third_coma = temp1.indexOf(',', second_coma + 1);

  String temp2 = temp1.substring(0, first_coma);
  func_Conductance = temp2.toFloat();
  Serial.println("filterd_Conductance: " + String(func_Conductance));

  String temp3 = temp1.substring(first_coma + 1, second_coma);
  func_Resistance = temp3.toFloat();
  Serial.println("filterd_Resistance: " + String(func_Resistance));

  String temp4 = temp1.substring(second_coma + 1, third_exclamation);
  func_Resistance_voltage = temp4.toFloat();
  Serial.println("filterd_Conductance_voltage: " +
                 String(func_Resistance_voltage));

  String temp5 = tempstr.substring(third_exclamation + 1, fourth_exclamation);
  func_snore_voltages = temp5.toFloat();
  Serial.println("filterd_snore_voltages: " + String(func_snore_voltages));
  // int data_input = 0;

  userID = user_id;
  filterd_temperatur = func_temperature;

  position = func_position;

  filterd_Conductance = func_Conductance;
  filterd_Resistance = func_Resistance;
  filterd_Conductance_voltage = func_Resistance_voltage;
  filterd_snore_voltages = func_snore_voltages;
}
