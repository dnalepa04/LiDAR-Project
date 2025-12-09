#include "Adafruit_VL53L1X.h"
#include <Stepper.h>

#define IRQ_PIN 2
#define XSHUT_PIN 3

const int stepsPerRevolution = 2048;  

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);
Stepper myStepper(stepsPerRevolution, 5,6,7,8);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println(F("Adafruit VL53L1X sensor demo"));

  Wire.begin();
  if (! vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }

  Serial.println(F("VL53L1X sensor OK!"));

  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);

  if (! vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }

  Serial.println(F("Ranging started"));

  vl53.setTimingBudget(50);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());

  // set speed at 5 rpm
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

    Serial.print(F("Angle: Distance: "));
    Serial.print(distance);
    Serial.println(" mm");

    // STOP MOTOR IF OBJECT IS BETWEEN 55–80mm
    if (distance >= 55 && distance <= 80) {
      Serial.println("Object detected - Stepper stopped");
      delay(100);  // short delay to prevent flooding Serial output
    } else if (distance > 80) {  
      // Continuous clockwise movement
      myStepper.step(128);  // move 128 step at a time
      // no long delay, but can put a very short delay if needed for smoothness
      delay(1);  
    }
    vl53.clearInterrupt();
  }
}

