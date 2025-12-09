#include "Stepper.h"
#include "Adafruit_VL53L1X.h"
 
const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
 
// ULN2003 Motor Driver Pins
#define IN1 5
#define IN2 6
#define IN3 7
#define IN4 8
 
// initialize the stepper library
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
 
#define IRQ_PIN 2
#define XSHUT_PIN 3
 
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

void setup() {
  // set the speed at 5 rpm
  myStepper.setSpeed(8.33); // 8.33
  // initialize the serial port
  Serial.begin(115200);
 
   Serial.begin(115200);
  while (!Serial) delay(10);
 
  Serial.println(F("Adafruit VL53L1X sensor demo"));
 
  Wire.begin();
  if (! vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1)       delay(10);
  }
  Serial.println(F("VL53L1X sensor OK!"));
 
  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);
 
  if (! vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1)       delay(10);
  }
  Serial.println(F("Ranging started"));
 
  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
  vl53.setTimingBudget(50);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());
 
  /*
  vl.VL53L1X_SetDistanceThreshold(100, 300, 3, 1);
  vl.VL53L1X_SetInterruptPolarity(0);
  */
 
if (vl53.dataReady()) {
    int16_t d = vl53.distance();
    vl53.clearInterrupt();
 
    // spin until distance is between 60 and 80
    while (d < 60 || d > 80) {
        myStepper.step(1);
 
        // update distance each time the sensor has new data
        if (vl53.dataReady()) {
            d = vl53.distance();
            vl53.clearInterrupt();
        }
    }
  }
}
 
void loop() {
  // // step one revolution in one direction:
  // Serial.println("clockwise");
  // myStepper.step(1000);
  // delay(1000);
 
  // // step one revolution in the other direction:
  // Serial.println("counterclockwise");
  // myStepper.step(-1000);
  // delay(1000);
 
  int16_t distance;
  for (int i; i < 2048; i +=1) {
      myStepper.step(1);
      if (vl53.dataReady()) {
        // new measurement for the taking
        distance = vl53.distance();
        if (distance == -1) {
          // something went wrong!
          Serial.print(F("Couldn't get distance: "));
          Serial.println(vl53.vl_status);
          return;
        }
        Serial.print(F("Distance: "));
        Serial.print(distance);
        Serial.println(" mm");
 
        // data is read out, time for another reading!
        vl53.clearInterrupt();
      }
  }
  // if (vl53.dataReady()) {
  //   // new measurement for the taking!
  //   distance = vl53.distance();
  //   if (distance == -1) {
  //     // something went wrong!
  //     Serial.print(F("Couldn't get distance: "));
  //     Serial.println(vl53.vl_status);
  //     return;
  //   }
  //   Serial.print(F("Distance: "));
  //   Serial.print(distance);
  //   Serial.println(" mm");
 
  //   // data is read out, time for another reading!
  //   vl53.clearInterrupt();
  // }
}