#include "Network.h"
#include "addons/TokenHelper.h"

#define WIFI_SSID "Revenant"
#define WIFI_PASSWORD "12345678"

#define API_KEY "AIzaSyBQWXIzgv66IqOBWpdNwEsf9nPWjnB9aCc" // mine
#define FIREBASE_PROJECT_ID "firestoretest-bb434"

// #define API_KEY "AIzaSyBj8RsT8ekXCs832OwJeFa9Nz-CTK-emWg" // wasey
// #define FIREBASE_PROJECT_ID "health-care-d5deb"
#define USER_EMAIL "hamid@fyp.com"
#define USER_PASSWORD "cui123"

static Network *instance = NULL;

Network::Network()
{
    instance = this;
}

void WiFiEventConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("WIFI CONNECTED! BUT WAIT FOR THE LOCAL IP ADDR");
}

void WiFiEventGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.print("LOCAL IP ADDRESS: ");
    Serial.println(WiFi.localIP());
    instance->firebaseInit();
}

void WiFiEventDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("WIFI DISCONNECTED!");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

// void FirestoreTokenStatusCallback(TokenInfo info){
//   Serial.printf("Token Info: type = %s, status = %s\n", getTokenType(info).c_str(), getTokenStatus(info).c_str());
// }

void Network::initWiFi()
{
    WiFi.disconnect();
    WiFi.onEvent(WiFiEventConnected, SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(WiFiEventGotIP, SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(WiFiEventDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void Network::firebaseInit()
{
    config.api_key = API_KEY;

    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    //  config.token_status_callback = FirestoreTokenStatusCallback;

    Firebase.begin(&config, &auth);
}

void Network::firestoreDataUpdate(double temp, double humi)
{
    if (WiFi.status() == WL_CONNECTED && Firebase.ready())
    {
        String documentPath = "patients/patient_1";

        FirebaseJson content;

        content.set("fields/temperature/doubleValue", String(temp).c_str());
        content.set("fields/humidity/doubleValue", String(humi).c_str());

        if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "temperature,humidity"))
        {
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
            return;
        }
        else
        {
            Serial.println(fbdo.errorReason());
        }

        if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw()))
        {
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
            return;
        }
        else
        {
            Serial.println(fbdo.errorReason());
        }
    }
}
