#include <Arduino.h>
#include <FreeRTOS.h>

void otra_tarea (void * parameter);

void setup() {
  Serial.begin(115200);

  xTaskCreate(otra_tarea, "otra_tarea", 1000, NULL, 1, NULL);

}

void loop() {
 Serial.println("Esta es una tarea de ESP32-S3");
 delay(1000);
}

void otra_tarea (void * parameter)
{
  for(;;)
  {
    Serial.println("Esta es otra tarea");
    delay(1000);
  }
vTaskDelete(NULL);
}