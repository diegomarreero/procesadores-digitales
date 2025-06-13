#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <RTClib.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define TFT_CS   7
#define TFT_DC   6
#define TFT_RST  5

const char* ssid = "ESP32-AP";
const char* password = "password";
const char* serverUrl = "http://192.168.4.1/data";

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

// Configuración gráficos
#define GRAPH_X 130
#define GRAPH_Y 45
#define GRAPH_WIDTH 180
#define GRAPH_HEIGHT 30
#define MAX_HISTORY GRAPH_WIDTH

// Historico para todos los sensores
float tempHistory[MAX_HISTORY];
float tempExtHistory[MAX_HISTORY];
float presHistory[MAX_HISTORY];
float humHistory[MAX_HISTORY];
int historyCount = 0;
int historyCountExt = 0;

Adafruit_AHTX0 aht10;
RTC_DS3231 rtc;

void drawGraph(int x, int y, int w, int h, float data[], int dataCount, uint16_t color);

unsigned long lastUpdate = 0;
const int refreshInterval = 1000;

void drawGraph(int x, int y, int w, int h, float data[], int dataCount, uint16_t color);

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  
  tft.setTextColor(ILI9341_YELLOW, ILI9341_CYAN);
  Wire.begin(8, 9);

  if (!aht10.begin() || !rtc.begin()) {
    tft.println("Error Sensores!");
    while(1);
  }


 WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  tft.fillScreen(ILI9341_BLACK);

  // Configuración inicial de la interfaz
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setTextSize(2);
  
  // Etiquetas
  tft.setCursor(10, 12);
  tft.print("INTERIOR");
  tft.setCursor(10, 50);
  tft.print("T:");
  
  tft.setCursor(10, 90);
  tft.print("EXTERIOR");
  tft.setCursor(10, 130);
  tft.print("T:");
  tft.setCursor(10, 170);
  tft.print("P:");
  tft.setCursor(10, 210);
  tft.print("H:");
  tft.setCursor(85, 130);
  tft.print(" C");

  // Bordes de los gráficos
  tft.drawRect(GRAPH_X, GRAPH_Y, GRAPH_WIDTH, GRAPH_HEIGHT, ILI9341_RED);
  tft.drawRect(130, 125, GRAPH_WIDTH, GRAPH_HEIGHT, ILI9341_RED);
  tft.drawRect(130, 165, GRAPH_WIDTH, GRAPH_HEIGHT, ILI9341_RED);
  tft.drawRect(130, 205, GRAPH_WIDTH, GRAPH_HEIGHT, ILI9341_RED);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    Serial.print("se ha desconectado");
  }

static int lastHour = -1, lastMinute = -1;
  static float lastTemp = -100;
  const int timeX = 250, timeY = 12;
  const int tempX = 35, tempY = 50;

  // Actualizar hora
  DateTime now = rtc.now();
  if(now.hour() != lastHour || now.minute() != lastMinute) {
    tft.fillRect(timeX, timeY, 100, 20, ILI9341_BLACK);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.setCursor(timeX, timeY);
    if(now.hour() < 10) tft.print('0');
    tft.print(now.hour());
    tft.print(':');
    if(now.minute() < 10) tft.print('0');
    tft.print(now.minute());
    lastHour = now.hour();
    lastMinute = now.minute();
  }

  // Actualizar temperatura interna
  sensors_event_t temp;
  aht10.getEvent(NULL, &temp);
  if(abs(temp.temperature - lastTemp) >= 0.1) {
    tft.fillRect(tempX, tempY, 80, 20, ILI9341_BLACK);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);    
    tft.setCursor(tempX, tempY);
    tft.print(temp.temperature, 1);
    tft.print(" C");
    tft.drawCircle(tempX + 55, tempY + 3, 2, ILI9341_GREEN);
    lastTemp = temp.temperature;
  }

  // Actualizar historico temperatura interna
  if(historyCount < MAX_HISTORY) {
    tempHistory[historyCount] = temp.temperature;
    historyCount++;
  } else {
    memmove(tempHistory, tempHistory + 1, (MAX_HISTORY - 1) * sizeof(float));
    tempHistory[MAX_HISTORY - 1] = temp.temperature;
  }

  // Obtener datos externos
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    
    if(http.GET() == HTTP_CODE_OK) {
      String payload = http.getString();
      StaticJsonDocument<200> doc;
      deserializeJson(doc, payload);
      
      float tempExt = doc["temperature"];
      float pres = doc["pressure"];
      float hum = doc["humidity"];

      // Actualizar valores en pantalla
      tft.setCursor(35, 130);
      tft.print(String(tempExt, 1));
      Serial.println(tempExt);
      tft.drawCircle(35 + 55, 130 + 3, 2, ILI9341_GREEN);
      
      tft.fillRect(35, 170, 60, 20, ILI9341_BLACK);
      tft.setCursor(35, 170);
      tft.print(String(pres, 0));
      Serial.println(pres);
      tft.setCursor(85, 170);
      tft.print("hPa");
      
      tft.setCursor(35, 210);
      tft.print(String(hum, 1));
      Serial.println(hum);
      Serial.print("---");
      tft.setCursor(85, 210);
      tft.print("%");

      // Actualizar historicos externos
      if(historyCountExt < MAX_HISTORY) {
        tempExtHistory[historyCountExt] = tempExt;
        presHistory[historyCountExt] = pres;
        humHistory[historyCountExt] = hum;
        historyCountExt++;
      } else {
        memmove(tempExtHistory, tempExtHistory + 1, (MAX_HISTORY - 1) * sizeof(float));
        memmove(presHistory, presHistory + 1, (MAX_HISTORY - 1) * sizeof(float));
        memmove(humHistory, humHistory + 1, (MAX_HISTORY - 1) * sizeof(float));
        tempExtHistory[MAX_HISTORY - 1] = tempExt;
        presHistory[MAX_HISTORY - 1] = pres;
        humHistory[MAX_HISTORY - 1] = hum;
      }
    }
    http.end();
    delay(2000);
  }

  // Dibujar todos los gráficos
  drawGraph(GRAPH_X, GRAPH_Y, GRAPH_WIDTH, GRAPH_HEIGHT, tempHistory, historyCount, ILI9341_PINK);     // Temp Interna
  drawGraph(130, 125, GRAPH_WIDTH, GRAPH_HEIGHT, tempExtHistory, historyCountExt, ILI9341_YELLOW);      // Temp Externa
  drawGraph(130, 165, GRAPH_WIDTH, GRAPH_HEIGHT, presHistory, historyCountExt, ILI9341_BLUE);           // Presión
  drawGraph(130, 205, GRAPH_WIDTH, GRAPH_HEIGHT, humHistory, historyCountExt, ILI9341_CYAN);            // Humedad

}


void drawGraph(int x, int y, int w, int h, float data[], int dataCount, uint16_t color) {
  tft.fillRect(x + 1, y + 1, w - 2, h - 2, ILI9341_BLACK);
  
  if(dataCount < 1) return;
  
  // Calcular rango dinámico
  float minVal = data[0];
  float maxVal = data[0];
  for(int i=1; i<dataCount; i++) {
    if(data[i] < minVal) minVal = data[i];
    if(data[i] > maxVal) maxVal = data[i];
  }
  
  // Margen para evitar división por cero
  minVal -= 0.5;
  maxVal += 0.5;
  if((maxVal - minVal) < 0.1) maxVal = minVal + 0.1;
  
  // Dibujar línea
  int prevX = x;
  int prevY = y + h - ((data[0] - minVal)/(maxVal - minVal)) * h;
  prevY = constrain(prevY, y, y + h - 1);
  
  for(int i=1; i<dataCount; i++) {
    int newX = x + i;
    int newY = y + h - ((data[i] - minVal)/(maxVal - minVal)) * h;
    newY = constrain(newY, y, y + h - 1);
    
    tft.drawLine(prevX, prevY, newX, newY, color);
    prevX = newX;
    prevY = newY;
  }
}