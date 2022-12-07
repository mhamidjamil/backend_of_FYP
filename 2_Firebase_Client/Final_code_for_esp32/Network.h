#ifndef Network_H_
#define Network_H_

#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <WiFi.h>

// #define RED 18
// #define GREEN 23
// #define BLUE 19
// pinMode(RED, OUTPUT);
// pinMode(GREEN, OUTPUT);
// pinMode(BLUE, OUTPUT);
class Network {
private:
  FirebaseData fbdo;
  FirebaseAuth auth;
  FirebaseConfig config;

  void firebaseInit();
  friend void WiFiEventConnected(WiFiEvent_t event, WiFiEventInfo_t info);
  friend void WiFiEventGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
  friend void WiFiEventDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
  friend void FirestoreTokenStatusCallback(TokenInfo info);

public:
  Network();
  void initWiFi();
  void firestoreDataUpdate(double temp, double humi);
  void firestoreDataUpdate(double temp, String position, double conductance,
                           double resistance, double resistive_voltages,
                           double snore_voltages);
};

#endif
