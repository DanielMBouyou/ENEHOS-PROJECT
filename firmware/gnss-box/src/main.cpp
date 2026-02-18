#include <Arduino.h>

static const uint32_t USB_BAUD = 115200;
static const uint32_t GNSS_BAUD = 9600;

void setup() {
  Serial.begin(USB_BAUD);
  while (!Serial && millis() < 3000) {
  }

  Serial1.begin(GNSS_BAUD);  // XIAO mapping expected: D7=RX, D6=TX
  Serial.println("GNSS box ready: UART bridge Serial1 <-> USB Serial");
}

void loop() {
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }

  while (Serial.available()) {
    Serial1.write(Serial.read());
  }
}
