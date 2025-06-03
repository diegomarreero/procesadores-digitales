#include <Arduino.h>  //DE MOMENTO NO FUNCIONA
#include <WiFi.h>
#include <ArduinoOTA.h>

const char* ssid = "MOVISTAR_0230_EXT";
const char* password = "7rxDbAhc4bEQJ8m2nrR2";

void setup() {
  Serial.begin(115200);


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
}