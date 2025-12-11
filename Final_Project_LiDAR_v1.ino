#include "Adafruit_VL53L1X.h"
#include <Stepper.h>
#include <Wire.h>
 
#define IRQ_PIN 2
#define XSHUT_PIN 3
 
const int stepsPerRevolution = 2052;
int16_t distance;
bool calibrated = false;
float angleStep = 0;
 
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);
Stepper myStepper(stepsPerRevolution, 5, 7, 6, 8);
 
void setup() {
  Serial.begin(115200);
  delay(100);
  // Serial.println("LiDAR Project Initialized");
 
  if (!vl53.begin()) {
    Serial.println("Failed to initialize VL53L1X!");
    while (1);
  }
  // begins ranging and timing budget
  bool x;
  x = vl53.startRanging();
  x = vl53.setTimingBudget(20);
  myStepper.setSpeed(4);  
  calibration();
 
  // 1 sec delay after calibration
  delay(2000);
}
// taken from Stepper_onRevolution example code
void loop() {
  angleStep = 2052.0 / 360.0;
  if(calibrated) {
    for (int i = -180; i < 180; i++) {
      // step one revolution in the other direction:
      myStepper.step(-6);
      while (!vl53.dataReady()) {
        // do nothing until new data is ready
      }
      if (vl53.dataReady()) {
      distance = vl53.distance();
      Serial.print(i);
      Serial.print(", ");
      Serial.println(distance);
      vl53.clearInterrupt();
      delay(30);
    }
    }
    for (int i = 180; i > -180; i--) {
      // step one revolution  in one direction:
      myStepper.step(6);
      while (!vl53.dataReady()) {
        // do nothing until new data is ready
      }
      if (vl53.dataReady()) {
      distance = vl53.distance();
      Serial.print(i);
      Serial.print(", ");
      Serial.println(distance);
      vl53.clearInterrupt();
      delay(30);
    }
    }
  }
}
 
void calibration() {
  // read initial distance
  distance = vl53.distance();
 
  while (distance <= 55 || distance >= 80) {
    myStepper.step(1);
    delay(5);
    // Serial.println(vl53.dataReady());
 
    if (vl53.dataReady()) {
      distance = vl53.distance();
      // Serial.print(F("Distance: "));
      // Serial.print(distance);
      // Serial.println(" mm");
      vl53.clearInterrupt();
    }
  }
  // this is a slop factor acounting for imperfect calibration
  myStepper.step(10);
  calibrated = true;
}
