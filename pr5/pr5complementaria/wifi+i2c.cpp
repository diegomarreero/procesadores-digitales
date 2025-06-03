#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Para la pantalla LCD
#include <RTClib.h>             // Para el RTC DS3231
#include <Adafruit_AHTX0.h>     // Para el sensor AHT10/AHT20
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Configuración de la pantalla LCD (0x27 es la dirección común)
LiquidCrystal_I2C lcd(0x27, 20, 4);  // 20 columnas x 4 filas

// Objetos para RTC y AHT10
RTC_DS3231 rtc;
Adafruit_AHTX0 aht10;

// Pines I2C para ESP32-S3
#define I2C_SDA 8
#define I2C_SCL 9

// Credenciales WiFi
const char* ssid = "vodafone1140";
const char* password = "EWCJYXQMNKJZNZ";

// Variables para almacenar los valores actuales
String lastDate = "";
String lastTime = "";
String lastTemp = "";
String lastHum = "";

// Variables para datos en crudo
int currentDay, currentMonth, currentYear;
int currentHour, currentMinute, currentSecond;
float currentTemperature;
float currentHumidity;

// Crear servidor web en puerto 80
AsyncWebServer server(80);

// Página web HTML
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Datos del Sensor</title>
  <style>
    body { font-family: Arial; text-align: center; margin: 0 auto; padding: 20px; }
    .data { font-size: 24px; margin: 15px; padding: 10px; background: #f0f0f0; border-radius: 5px; }
  </style>
  <script>
    function updateData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          document.getElementById('date').innerHTML = data.fecha;
          document.getElementById('time').innerHTML = data.hora;
          document.getElementById('temp').innerHTML = data.temperatura + " °C";
          document.getElementById('hum').innerHTML = data.humedad + " %";
        });
    }
    setInterval(updateData, 1000);
  </script>
</head>
<body>
  <h1>Datos del Sensor</h1>
  <div class="data">Fecha: <span id="date">-</span></div>
  <div class="data">Hora: <span id="time">-</span></div>
  <div class="data">Temperatura: <span id="temp">-</span></div>
  <div class="data">Humedad: <span id="hum">-</span></div>
</body>
</html>
)rawliteral";

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);
  
  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi...");

  // Conectar a WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Mostrar IP en LCD
  lcd.clear();
  lcd.print("IP:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(5000);
  lcd.clear();

  // Configurar rutas del servidor web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = String() + 
      "{\"fecha\":\"" + lastDate + 
      "\",\"hora\":\"" + lastTime + 
      "\",\"temperatura\":" + String(currentTemperature, 1) + 
      ",\"humedad\":" + String(currentHumidity, 1) + "}";
    request->send(200, "application/json", json);
  });

  // Iniciar servidor
  server.begin();

  // Inicializar RTC
  if (!rtc.begin()) {
    lcd.print("Error RTC!");
    while (1);
  }

  // Inicializar AHT10
  aht10.begin();
}

void loop() {
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();

    // Leer datos del RTC
    DateTime now = rtc.now();
    currentDay = now.day();
    currentMonth = now.month();
    currentYear = now.year();
    currentHour = now.hour();
    currentMinute = now.minute();
    currentSecond = now.second();

    // Leer datos del AHT10
    sensors_event_t humidity, temp;
    aht10.getEvent(&humidity, &temp);
    currentTemperature = temp.temperature;
    currentHumidity = humidity.relative_humidity;

    // Formatear strings para LCD
    String currentDate = String(currentDay) + "/" + String(currentMonth) + "/" + String(currentYear);
    String currentTime = String(currentHour) + ":" + (currentMinute < 10 ? "0" : "") + String(currentMinute) + ":" + (currentSecond < 10 ? "0" : "") + String(currentSecond);
    String currentTemp = "Temp: " + String(currentTemperature, 1) + " C";
    String currentHum = "Hum:  " + String(currentHumidity, 1) + " %";

    // Actualizar LCD solo si hay cambios
    if (currentDate != lastDate) {
      lcd.setCursor(0, 0);
      lcd.print(currentDate);
      lastDate = currentDate;
    }

    if (currentTime != lastTime) {
      lcd.setCursor(0, 1);
      lcd.print(currentTime);
      lastTime = currentTime;
    }

    if (currentTemp != lastTemp) {
      lcd.setCursor(0, 2);
      lcd.print(currentTemp);
      lastTemp = currentTemp;
    }

    if (currentHum != lastHum) {
      lcd.setCursor(0, 3);
      lcd.print(currentHum);
      lastHum = currentHum;
    }
  }
}