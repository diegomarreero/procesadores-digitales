#include <SPI.h>
#include <SD.h>

// Prueba con estos pines alternativos:
#define CS_PIN    5   // Opciones comunes: 5, 13, 21
#define SCK_PIN   12
#define MISO_PIN  13
#define MOSI_PIN  11

SPIClass spi = SPIClass(FSPI); // Usa SPI1 (FSPI)
File myFile;

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Configura SPI con los pines
  spi.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

  Serial.print("Iniciando SD... ");
  
  // Intenta inicializar con velocidad reducida (1 MHz)
  if (!SD.begin(CS_PIN)) { // <- ¡Velocidad reducida!
    Serial.println("Fallo. Verifica:");
    Serial.println("- Pines y conexiones");
    Serial.println("- Formato FAT32");
    Serial.println("- Resistencia pull-up");
    while (1);
  }
  Serial.println("Correcto");

  // Leer archivo...
  myFile = SD.open("/myFile.txt");
  if (myFile) {
    Serial.println("Contenido:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("Error abriendo archivo");
  }
}

void loop() {}

void loop()
{
  
}