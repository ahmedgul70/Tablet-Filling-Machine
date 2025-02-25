
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>


#define DATABASE_URL "hms-2021-default-rtdb.firebaseio.com" //"tablet-filling-default-rtdb.firebaseio.com"
#define DATABASE_SECRET "AIzaSyDK35I88bGOoc3jIdw5I12wuFH0iqlBv1A" //"d74dXVC5GNOdRDhbKT2BH5ZKMuEwGQUqEJrZNb8e"

#define API_KEY ""
#define USER_EMAIL ""
#define USER_PASSWORD ""


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void connectWifi();
void fbaseBegin();
void fbaseSend(bool t = 0);
bool fbaseRead(bool t = 0);



bool fbaseRead(bool t)
{
  static uint32_t ptime = 5000;

  if (Firebase.ready() && (millis() - ptime > 1000) || t)
  {
    Serial.println("fbaseRead");

    if (Firebase.getInt(fbdo, "/tabletFilling/tablet"))
    {
      if (fbdo.dataType() == "int") {
        tablet = fbdo.intData();
        if (tablet > 0) tablet = 1;
        Serial.println(tablet);
      }
    }
    else Serial.println(fbdo.errorReason());

    ptime = millis();
    return 1;
  }
  return 0;
}




void fbaseSend(bool t)
{
  static uint32_t ptime;
  if (Firebase.ready() && (millis() - ptime > 1000 || t))
  {
    Serial.print("Set int async... ");

    //Firebase.setBoolAsync(fbdo, "/deviceStatus", true);
    Firebase.setIntAsync(fbdo, "/tabletFilling/stage", stage);
    Firebase.setIntAsync(fbdo, "/tabletFilling/bottleCount", bottleCount);
    Firebase.setIntAsync(fbdo, "/tabletFilling/finish", finish);

    Serial.println("\nok");

    ptime = millis();
  }
}




void fbaseBegin()
{
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  config.signer.tokens.legacy_token = DATABASE_SECRET;

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

#if defined(ESP8266)
  fbdo.setBSSLBufferSize(512, 2048);
#endif

  delay(200);
  if (Firebase.ready()) {
    //Firebase.setIntAsync(fbdo, "/tabletFilling/tablet", 0);
    Firebase.setIntAsync(fbdo, "/tabletFilling/stage", 0);
    Firebase.setIntAsync(fbdo, "/tabletFilling/bottleCount", 0);
    Firebase.setIntAsync(fbdo, "/tabletFilling/finish", 0);
  }
}



void connectWifi()
{
  WiFi.mode(WIFI_OFF);
  delay(500);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(ssid, pass);

  Serial.print("\nConnecting");

  while (WiFi.status() != WL_CONNECTED && (millis() < 10000)) {
    delay(500);
    Serial.print(".");
  }

  Serial.println((WiFi.status() == WL_CONNECTED) ? "\nConnected to WiFi network" : "\nwifi not connected skiped process");

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}
