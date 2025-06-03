#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Para la pantalla LCD
#include <RTClib.h>             // Para el RTC DS3231
#include <Adafruit_AHTX0.h>     // Para el sensor AHT10/AHT20

// Configuración de la pantalla LCD (0x27 es la dirección común)
LiquidCrystal_I2C lcd(0x27, 20, 4);  // 20 columnas x 4 filas

// Objetos para RTC y AHT10
RTC_DS3231 rtc;
Adafruit_AHTX0 aht10;

// Pines I2C para ESP32-S3 (ajusta según tu placa)
#define I2C_SDA 8
#define I2C_SCL 9

// Variables para almacenar los valores anteriores
String lastDate = "";
String lastTime = "";
String lastTemp = "";
String lastHum = "";

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);
  
  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");

  // Inicializar RTC
  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("Error RTC!");
    while (1);
  }

  // Ajustar hora si el RTC perdió energía (descomenta para configurar)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Inicializar AHT10
  if (!aht10.begin()) {  // Inicia el sensor AHT10
    lcd.clear();
    lcd.print("Error AHT10!");
    while (1);
  }

  // Esperar un momento antes de comenzar
  delay(2000);
  lcd.clear();
}

void loop() {
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= 1000) {  // Actualizar cada 1 segundo
    lastUpdate = millis();

    // Leer datos del RTC
    DateTime now = rtc.now();
    
    // Leer datos del AHT10
    sensors_event_t humidity, temp;
    aht10.getEvent(&humidity, &temp);  // Obtiene temperatura y humedad

    // Formatear fecha y hora
    String currentDate = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
    String currentTime = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    String currentTemp = "Temp: " + String(temp.temperature, 1) + " C";
    String currentHum = "Hum:  " + String(humidity.relative_humidity, 1) + " %";

    // Actualizar solo si los valores han cambiado
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