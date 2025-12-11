// Authors: Nate Seibold, Daniel Nalepa
// Date: 12/12/2025
// Optoelectronics Final Arduino Code

// import libraries
#include "Adafruit_VL53L1X.h"
#include <Stepper.h>
#include <Wire.h>

// define ToF pins
#define IRQ_PIN 2
#define XSHUT_PIN 3

// declare variables
const int stepsPerRevolution = 2052;
int16_t distance;
bool calibrated = false;

// initialize ToF and stepper
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);
Stepper myStepper(stepsPerRevolution, 5, 7, 6, 8);
 
void setup() {
  Serial.begin(115200);
  delay(100);

  // warning if ToF can't boot
  if (!vl53.begin()) {
    Serial.println("Failed to initialize VL53L1X!");
    while (1);
  }
 
  // sets start-up parameters for Tof and stepper
  bool x;
  x = vl53.startRanging();
  x = vl53.setTimingBudget(20);
  myStepper.setSpeed(4);  

  // run calibration sequence
  calibration();
 
  // 2 second delay after calibration
  delay(2000);
}

// taken from Stepper_onRevolution example code
void loop() {
  // ensure calibration
  if(calibrated) {
    // cycle through one rotation clockwise, one degree increment
    for (int i = -180; i < 180; i++) {
      // step one degree (actual is 5.7)
      myStepper.step(-6);

      // hold motor until ToF has refreshed
      while (!vl53.dataReady()) {
      }

      // with data ready, gather distance data
      if (vl53.dataReady()) {
        distance = vl53.distance();
        Serial.print(i); // print angle in degrees
        Serial.print(", ");
        Serial.println(distance); // print distance in mm
        vl53.clearInterrupt(); // clear for next data point
        delay(30);
      }
    }
   
    // cycle through one rotation counter-clockwise, one degree increment
    for (int i = 180; i > -180; i--) {
      // step one degree (actual is 5.7)
      myStepper.step(6);

      // hold motor until ToF has refreshed
      while (!vl53.dataReady()) {
      }
      // with data ready, gather distance data
      if (vl53.dataReady()) {
        distance = vl53.distance();
        Serial.print(i); // print angle in degrees
        Serial.print(", ");
        Serial.println(distance); // print distance in mm
        vl53.clearInterrupt(); // clear for next data point
        delay(30);
      }
    }
  }
}

// define calibration function
// sensor needs to know 180 degrees when starting
void calibration() {
  // read initial distance
  distance = vl53.distance();
 
  // arm is known to be in this range, keep rotating until this distance is found
  while (distance <= 55 || distance >= 80) {
    // perform small steps, we don't know where the sensor is 
    myStepper.step(1);
    delay(5);

    // remeasure distance
    if (vl53.dataReady()) {
      distance = vl53.distance();
      vl53.clearInterrupt();
    }
  }
 
  // the sensor will stop on the outer edge of the arm, add an extra 1.5 degrees to account
  myStepper.step(10);

  // update flag so loop can run
  calibrated = true;
}

