#include <Arduino.h>

// APARTADO A

// Definiciones de pines
const int ledPin = 47;          // Pin del LED
const int buttonUpPin = 0;     // Pin del pulsador para aumentar la frecuencia
const int buttonDownPin = 2;   // Pin del pulsador para disminuir la frecuencia

// Variables de estado
volatile int blinkFrequency = 500;  // Frecuencia inicial de parpadeo en ms
volatile bool ledState = LOW;       // Estado del LED

// Variables para el filtrado de rebotes
unsigned long lastDebounceTimeUp = 0;   // Último tiempo de rebote para el pulsador de subir
unsigned long lastDebounceTimeDown = 0; // Último tiempo de rebote para el pulsador de bajar
const unsigned long debounceDelay = 50; // Tiempo de debounce en ms

// Prototipos de funciones
void IRAM_ATTR onTimer();
void checkButtons();

// Configuración del timer
hw_timer_t *timer = NULL;

void setup() {
  // Configuración de pines
  pinMode(ledPin, OUTPUT);
  pinMode(buttonUpPin, INPUT_PULLUP);
  pinMode(buttonDownPin, INPUT_PULLUP);

  // Inicialización del timer
  timer = timerBegin(0, 80, true);  // Timer 0, prescaler de 80 (1 MHz), cuenta ascendente
  timerAttachInterrupt(timer, &onTimer, true);  // Asociar la función de interrupción
  timerAlarmWrite(timer, blinkFrequency * 1000, true);  // Configurar el período del timer en microsegundos
  timerAlarmEnable(timer);  // Habilitar la alarma del timer
}

void loop() {
  // Verificar el estado de los pulsadores
  checkButtons();
}

// Función de interrupción del timer
void IRAM_ATTR onTimer() {
  ledState = !ledState;  // Cambiar el estado del LED
  digitalWrite(ledPin, ledState);  // Actualizar el estado del LED
}

//os pu Función para verificar llsadores con filtrado de rebotes
void checkButtons() {
  // Leer el estado de los pulsadores
  bool buttonUpState = digitalRead(buttonUpPin);
  bool buttonDownState = digitalRead(buttonDownPin);

  // Verificar el pulsador de subir frecuencia
  if (buttonUpState == LOW) {
    if (millis() - lastDebounceTimeUp > debounceDelay) {
      blinkFrequency -= 50;  // Aumentar la frecuencia (disminuir el tiempo)
      if (blinkFrequency < 50) blinkFrequency = 50;  // Limitar la frecuencia mínima
      timerAlarmWrite(timer, blinkFrequency * 1000, true);  // Actualizar el período del timer
      lastDebounceTimeUp = millis();  // Actualizar el tiempo de debounce
    }
  }

  // Verificar el pulsador de bajar frecuencia
  if (buttonDownState == LOW) {
    if (millis() - lastDebounceTimeDown > debounceDelay) {
      blinkFrequency += 50;  // Disminuir la frecuencia (aumentar el tiempo)
      if (blinkFrequency > 1000) blinkFrequency = 1000;  // Limitar la frecuencia máxima
      timerAlarmWrite(timer, blinkFrequency * 1000, true);  // Actualizar el período del timer
      lastDebounceTimeDown = millis();  // Actualizar el tiempo de debounce
    }
  }
}

void loop() {
  // Verificar el estado de los pulsadores
  checkButtons();
}

// Función de interrupción del timer
void IRAM_ATTR onTimer() {
  ledState = !ledState;  // Cambiar el estado del LED
  digitalWrite(ledPin, ledState);  // Actualizar el estado del LED
}

// Función para verificar los pulsadores con filtrado de rebotes
void checkButtons() {
  // Leer el estado de los pulsadores
  bool buttonUpState = digitalRead(buttonUpPin);    // Botón BOOT
  bool buttonDownState = digitalRead(buttonDownPin); // Pulsador externo (opcional)

  // Verificar el botón BOOT (aumentar frecuencia)
  if (buttonUpState == LOW) {  // El botón BOOT está activo en LOW
    if (millis() - lastDebounceTimeUp > debounceDelay) {
      blinkFrequency -= 50;  // Aumentar la frecuencia (disminuir el tiempo)
      if (blinkFrequency < 50) blinkFrequency = 50;  // Limitar la frecuencia mínima
      timerAlarmWrite(timer, blinkFrequency * 1000, true);  // Actualizar el período del timer
      lastDebounceTimeUp = millis();  // Actualizar el tiempo de debounce
    }
  }

  // Verificar el pulsador externo (disminuir frecuencia)
  if (buttonDownState == LOW) {  // El pulsador externo está activo en LOW
    if (millis() - lastDebounceTimeDown > debounceDelay) {
      blinkFrequency += 50;  // Disminuir la frecuencia (aumentar el tiempo)
      if (blinkFrequency > 1000) blinkFrequency = 1000;  // Limitar la frecuencia máxima
      timerAlarmWrite(timer, blinkFrequency * 1000, true);  // Actualizar el período del timer
      lastDebounceTimeDown = millis();  // Actualizar el tiempo de debounce
    }
  }
}