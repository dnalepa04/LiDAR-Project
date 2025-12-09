#include "Adafruit_VL53L1X.h"
#include <Stepper.h>

#define IRQ_PIN 2
#define XSHUT_PIN 3

const int stepsPerRevolution = 2048;  // steps for full 360° rotation  
const int angle = 90;
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);
Stepper myStepper(stepsPerRevolution, 5, 6, 7, 8);

bool calibrated = false;  // flag to check if we reached the calibration point

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println(F("Adafruit VL53L1X sensor demo"));

  Wire.begin();
  if (!vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }

  Serial.println(F("VL53L1X sensor OK!"));
  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);

  if (!vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }

  Serial.println(F("Ranging started"));

  vl53.setTimingBudget(50);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());

  // set speed
  myStepper.setSpeed(8.33);
}

void loop() {
  int16_t distance;

  if (vl53.dataReady()) {
    distance = vl53.distance();
    if (distance == -1) {
      Serial.print(F("Couldn't get distance: "));
      Serial.println(vl53.vl_status);
      return;
    }
     
    Serial.print(angle);
    Serial.print(", ");
    Serial.println(distance);

    if (!calibrated) {
      // STOP MOTOR IF OBJECT IS BETWEEN 55–80mm
      if (distance >= 55 && distance <= 80) {
        Serial.println("Calibration point reached - starting rotation sequence");
        calibrated = true;  // set flag
        delay(500); // optional short pause
      }
    } //else {
    //   // Start back-and-forth rotation
    //   Serial.println("Rotating clockwise 360°");
    //   myStepper.step(stepsPerRevolution);  // clockwise 360°
      
    //   delay(500); // short pause
    //   Serial.println("Rotating counterclockwise 360°");
    //   myStepper.step(-stepsPerRevolution); // counterclockwise 360°
    //   delay(500); // short pause
    // }

    vl53.clearInterrupt();
  }
}
