#include <Arduino.h>

int led = 12;

void led_on (void * parameter);

void led_off (void* parameter);

SemaphoreHandle_t encendido;

SemaphoreHandle_t apagado;

void setup()
{
  pinMode(led, OUTPUT);

  encendido = xSemaphoreCreateBinary();

  apagado = xSemaphoreCreateBinary();

  xSemaphoreGive(apagado);

  xSemaphoreGive(encendido);

  Serial.begin(115200);

  xTaskCreate(led_on, "led_on", 1000, NULL, 1, NULL);

  xTaskCreate(led_off, "led_off", 1000, NULL, 1, NULL);

}

void loop()
{

}


void led_on (void * parameter)
{
  (void) parameter;
  pinMode(led, OUTPUT);
  for(;;)
  {
    if(xSemaphoreTake(apagado, portMAX_DELAY) == pdPASS)
    {
      digitalWrite(led, HIGH);
      Serial.println("LED ENCENDIDO");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      xSemaphoreGive(encendido);
    }
  }
  vTaskDelete(NULL);
}

void led_off (void * parameter)
{
  (void) parameter;
  pinMode(led, OUTPUT);
  for(;;)
  {
    if(xSemaphoreTake(encendido, portMAX_DELAY) == pdPASS)
    {
      digitalWrite(led, LOW);
      Serial.println("LED APAGADO");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      xSemaphoreGive(apagado);
    }
  }
  vTaskDelete(NULL);
}