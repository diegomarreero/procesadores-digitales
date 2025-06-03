#include "AudioGeneratorAAC.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourcePROGMEM.h"
#include "sampleaac.h"  // Ensure your AAC file is properly formatted

// Pin configuration for ESP32-S3
#define I2S_BCLK 36
#define I2S_LRC 37
#define I2S_DOUT 38

AudioFileSourcePROGMEM *file;
AudioGeneratorAAC *aac;
AudioOutputI2S *out;

void setup() {
  Serial.begin(115200);
  
  // Initialize audio objects
  file = new AudioFileSourcePROGMEM(sampleaac, sizeof(sampleaac));
  out = new AudioOutputI2S();
  aac = new AudioGeneratorAAC();

  // Configure I2S output
  out->SetGain(1);  // Reduced gain to prevent distortion
  out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  // Start audio generation
  if (!aac->begin(file, out)) {
    Serial.println("Failed to start AAC decoder!");
    while (1);  // Halt on failure
  }
  
  Serial.println("Audio playback started");
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
    file->seek(0, SEEK_SET);  // Reset to start of audio data
    aac->begin(file, out);
  }
}pr7