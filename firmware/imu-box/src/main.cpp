#include <Arduino.h>
#include <Wire.h>

static const uint32_t USB_BAUD = 115200;
static uint32_t lastScanMs = 0;

void scanI2C() {
  uint8_t found = 0;
  Serial.println("I2C scan start");

  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("  Found device at 0x");
      if (address < 16) {
        Serial.print('0');
      }
      Serial.println(address, HEX);
      found++;
    }
  }

  Serial.print("I2C scan done, found=");
  Serial.println(found);
}

void setup() {
  Serial.begin(USB_BAUD);
  while (!Serial && millis() < 3000) {
  }

  Wire.begin();  // XIAO mapping expected: D4=SDA, D5=SCL
  Serial.println("IMU box ready: I2C scanner");
}

void loop() {
  if (millis() - lastScanMs >= 2000) {
    lastScanMs = millis();
    scanI2C();
  }
}
