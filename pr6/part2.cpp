#include <SPI.h>
#include <MFRC522.h>

// Pines SPI (ESP32-S3)
#define RFID_CS_PIN   5   // GPIO5
#define RST_PIN       4   // GPIO4

MFRC522 rfid(RFID_CS_PIN, RST_PIN); // Constructor

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Inicializa SPI con pines personalizados para ESP32-S3:
  SPI.begin(12, 13, 11, RFID_CS_PIN); // SCK=12, MISO=13, MOSI=11, SS=5

  rfid.PCD_Init(); 
  Serial.println("Acerca una tarjeta RFID...");

}

void loop() {
  // Detección mejorada
  if (!rfid.PICC_IsNewCardPresent()) {
    return; 
  }
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Mostrar UID
  Serial.print("UID:");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}