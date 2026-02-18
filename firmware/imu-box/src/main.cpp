#include <Arduino.h>
#include <Wire.h>

#include "SparkFun_BNO08x_Arduino_Library.h"

static const uint32_t USB_BAUD = 115200;
static const uint8_t BNO08X_ADDR_PRIMARY = 0x4B;
static const uint8_t BNO08X_ADDR_SECONDARY = 0x4A;
static const int BNO08X_INT = D9;  // User wiring: INT -> D9
static const int BNO08X_RST = D7;  // User wiring: RST -> D7
static const uint16_t REPORT_INTERVAL_MS = 50;  // 20 Hz

BNO08x imu;
static bool imuReady = false;
static uint8_t imuAddr = 0;
static uint32_t lastRetryMs = 0;
static bool imuUsingControlPins = true;

static float toHeadingDegrees(float yawDeg) {
  float heading = fmodf(yawDeg, 360.0f);
  if (heading < 0.0f) {
    heading += 360.0f;
  }
  return heading;
}

static bool configureReports() {
  if (!imu.enableRotationVector(REPORT_INTERVAL_MS)) {
    return false;
  }
  return true;
}

static bool beginImuOnAddress(uint8_t addr, bool withControlPins) {
  const bool ok = withControlPins ? imu.begin(addr, Wire, BNO08X_INT, BNO08X_RST)
                                  : imu.begin(addr, Wire, -1, -1);
  if (!ok) return false;

  if (!configureReports()) {
    return false;
  }

  imuAddr = addr;
  imuUsingControlPins = withControlPins;
  return true;
}

static bool beginImu() {
  if (beginImuOnAddress(BNO08X_ADDR_PRIMARY, false)) {
    return true;
  }
  if (beginImuOnAddress(BNO08X_ADDR_SECONDARY, false)) {
    return true;
  }
  if (beginImuOnAddress(BNO08X_ADDR_PRIMARY, true)) {
    return true;
  }
  if (beginImuOnAddress(BNO08X_ADDR_SECONDARY, true)) {
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(USB_BAUD);
  while (!Serial && millis() < 3000) {
  }

  Wire.begin();  // XIAO default I2C pins: D4 (SDA), D5 (SCL)
  Wire.setClock(400000);

  Serial.println("imu_boot");
  Serial.println("csv:millis,roll_deg,pitch_deg,yaw_deg,heading_deg");
  Serial.println("imu_trying:bno08x_addr_0x4B_then_0x4A");

  imuReady = beginImu();
  if (!imuReady) {
    Serial.println("imu_error:bno08x_not_ready");
  } else {
    Serial.print("imu_ready:addr=0x");
    Serial.print(imuAddr, HEX);
    Serial.print(",ctrl_pins=");
    Serial.println(imuUsingControlPins ? "on" : "off");
  }
}

void loop() {
  if (!imuReady) {
    if (millis() - lastRetryMs > 2000) {
      lastRetryMs = millis();
      Serial.println("imu_retry:trying_to_connect");
      imuReady = beginImu();
      if (imuReady) {
        Serial.print("imu_ready:addr=0x");
        Serial.print(imuAddr, HEX);
        Serial.print(",ctrl_pins=");
        Serial.println(imuUsingControlPins ? "on" : "off");
      }
    }
    delay(10);
    return;
  }

  if (imu.wasReset()) {
    Serial.println("imu_notice:reset_detected");
    if (!configureReports()) {
      Serial.println("imu_error:reconfigure_failed");
      imuReady = false;
      delay(200);
      return;
    }
  }

  if (!imu.getSensorEvent()) {
    delay(2);
    return;
  }

  if (imu.getSensorEventID() != SENSOR_REPORTID_ROTATION_VECTOR) {
    return;
  }

  const float rollDeg = imu.getRoll() * 180.0f / PI;
  const float pitchDeg = imu.getPitch() * 180.0f / PI;
  const float yawDeg = imu.getYaw() * 180.0f / PI;
  const float headingDeg = toHeadingDegrees(yawDeg);

  Serial.print(millis());
  Serial.print(",");
  Serial.print(rollDeg, 2);
  Serial.print(",");
  Serial.print(pitchDeg, 2);
  Serial.print(",");
  Serial.print(yawDeg, 2);
  Serial.print(",");
  Serial.println(headingDeg, 2);
}
