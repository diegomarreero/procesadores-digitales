#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceSD.h"
#include <SD.h>
#include <SPI.h>

// Configuración SD Card - Versión Corregida
#define CS_PIN    5
#define SCK_PIN   36  // Cambiado a pin válido para SPI
#define MISO_PIN  37  // Cambiado a pin válido para SPI
#define MOSI_PIN  35  // Cambiado a pin válido para SPI

// Configuración I2S
#define I2S_BCLK 17
#define I2S_LRC  18
#define I2S_DOUT 8

SPIClass *spi = nullptr;  // Usaremos SPI nativo
AudioFileSourceSD *file;
AudioGeneratorWAV *wav;
AudioOutputI2S *out;

void listSDContents() {
  File root = SD.open("/");
  if(!root){
    Serial.println("Error al abrir directorio raíz");
    return;
  }
  
  Serial.println("Contenido de la tarjeta SD:");
  File entry = root.openNextFile();
  while(entry){
    Serial.printf("  %s (%d bytes)\n", entry.name(), entry.size());
    entry.close();
    entry = root.openNextFile();
  }
  root.close();
}

void setup() {
  Serial.begin(115200);
  while(!Serial); // Esperar conexión serial

  // 1. Configuración SPI mejorada
  spi = new SPIClass(HSPI);  // Usar HSPI nativo
  spi->begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  
  // Configuración crítica de pines
  pinMode(MISO_PIN, INPUT_PULLUP);  // Pull-up esencial
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  delay(100);

  Serial.println("Inicializando tarjeta SD...");
  
  // 2. Inicialización SD con múltiples intentos
  for(int i = 0; i < 5; i++) {
    if(SD.begin(CS_PIN, *spi, 4000000)) {  // 4MHz
      Serial.println("SD inicializada correctamente");
      listSDContents();
      break;
    }
    Serial.printf("Intento %d fallado\n", i+1);
    delay(500);
    
    // Alternar CS para resetear tarjeta
    digitalWrite(CS_PIN, HIGH);
    delay(100);
    digitalWrite(CS_PIN, LOW);
    delay(100);
    digitalWrite(CS_PIN, HIGH);
  }

  if(!SD.cardType()) {
    Serial.println("Fallo definitivo en SD. Verifica:");
    Serial.println("1. Conexiones (SCK/MISO/MOSI/CS)");
    Serial.println("2. Formato FAT32");
    Serial.println("3. Alimentación 3.3V estable");
    while(true);
  }

  // Resto del código de audio...
  // [Mantener igual desde la verificación de archivo WAV]


// [Mantener loop() igual]

  // Initialize SD card
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card initialization failed!");
    while (1); // Halt if SD card fails
  }
  Serial.println("SD Card initialized");

  // Initialize audio components
  out = new AudioOutputI2S();
  file = new AudioFileSourceSD("/cangrinaje.wav"); // Change to your filename
  aac = new AudioGeneratorAAC();

  // Configure I2S
  out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  out->SetGain(0.5); // Adjust volume here

  // Verify file existence
  if (!file->isOpen()) {
    Serial.println("Failed to open audio file!");
    while (1); // Halt if file not found
  }

  // Start playback
  if (!aac->begin(file, out)) {
    Serial.println("Failed to start AAC decoder!");
    while (1);
  }
  Serial.println("Starting playback...");
}

void loop() {
  if (aac->isRunning()) {
    if (!aac->loop()) {
      aac->stop();
      Serial.println("Playback finished");
    }
  } else {
    Serial.println("Restarting playback...");
    delay(1000);
    file->close();
    file->open("/test.aac");
    aac->begin(file, out);
  }
}