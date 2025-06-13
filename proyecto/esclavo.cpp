#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

const char* ssid = "ESP32-AP";
const char* password = "password";
const char* serverURL = "http://192.168.4.1/submit";

Adafruit_BME280 bme; // Objeto del sensor BME280

void setup() {
  Serial.begin(115200);
  
  // Inicializar sensor BME280
  if (!bme.begin(0x76)) { // Usar 0x77 si falla
    Serial.println("Error al iniciar BME280!");
    while (1);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Leer datos reales del sensor
    float temp = bme.readTemperature();    // °C
    float pres = bme.readPressure() / 100; // Convertir Pa a hPa
    float hum = bme.readHumidity();        // % Humedad relativa

    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String data = "temp=" + String(temp) + 
                  "&pres=" + String(pres) + 
                  "&hum=" + String(hum);
                  
    int httpCode = http.POST(data);
    
    if (httpCode == HTTP_CODE_OK) {
      Serial.print("Datos enviados: ");
      Serial.println(data);
    }
    
    http.end();
  }
  delay(1000); 
}