#include <AFMotor.h>       // Adafruit Motor Shield library
#include <Servo.h>         // Servo Motor library
#include <NewPing.h>       // Ultrasonic sensor library

// Pin definitions for the Ultrasonic Sensor
#define TRIG_PIN A0        // TRIG pin for ultrasonic sensor
#define ECHO_PIN A1        // ECHO pin for ultrasonic sensor

// Constants for the robot behavior
#define MAX_DISTANCE 300   // Maximum distance the ultrasonic sensor can measure (in cm)
#define MAX_SPEED 160      // Maximum speed of DC traction motors (scaled for power saving)
#define MAX_SPEED_OFFSET 40 // Offset to allow for motor power differences
#define COLL_DIST 30       // Collision distance threshold (in cm) to stop and reverse
#define TURN_DIST (COLL_DIST + 20) // Threshold to veer away from obstacles

// Initialize the ultrasonic sensor
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// Create motors using the Adafruit Motor Shield library (Motor 1-4)
AF_DCMotor leftMotor1(1, MOTOR12_1KHZ); // Motor 1 on M1 output
AF_DCMotor leftMotor2(2, MOTOR12_1KHZ); // Motor 2 on M2 output
AF_DCMotor rightMotor1(3, MOTOR34_1KHZ); // Motor 3 on M3 output
AF_DCMotor rightMotor2(4, MOTOR34_1KHZ); // Motor 4 on M4 output

Servo myservo;  // Create servo object to control the servo motor

// Variables to store sensor data
int leftDistance, rightDistance;
int curDist = 0;
String motorSet = "";
int speedSet = 0;

void setup() {
  myservo.attach(10);  // Attach servo to pin 10
  myservo.write(90);   // Initial servo position (90 degrees facing forward)
  delay(1000);         // Wait for one second
}

void loop() {
  myservo.write(90);   // Move servo to 90 degrees (forward)
  delay(90);            // Small delay before reading distance

  curDist = readPing(); // Read the distance from the ultrasonic sensor
  if (curDist < COLL_DIST) {
    changePath();       // If an obstacle is detected, change path
  }
  moveForward();        // Move forward if no obstacle detected
  delay(500);           // Wait for 500 ms
}

void changePath() {
  moveStop();           // Stop movement
  myservo.write(36);    // Move servo to check the right
  delay(500);
  rightDistance = readPing(); // Get the distance to the right
  delay(500);

  myservo.write(144);   // Move servo to check the left
  delay(700);
  leftDistance = readPing(); // Get the distance to the left
  delay(500);

  myservo.write(90);    // Return servo to center
  delay(100);

  compareDistance();    // Compare left and right distances to determine the best path
}

void compareDistance() {
  // If left side is less obstructed, turn left; if right side is less obstructed, turn right
  if (leftDistance > rightDistance) {
    turnLeft();
  } else if (rightDistance > leftDistance) {
    turnRight();
  } else {  // If both sides are equally blocked, turn around
    turnAround();
  }
}

int readPing() {
  delay(70);            // Delay before reading sensor
  unsigned int uS = sonar.ping(); // Read the ultrasonic sensor
  int cm = uS / US_ROUNDTRIP_CM;  // Convert the sensor reading to cm
  return cm;
}

void moveStop() {
  // Stop all motors
  leftMotor1.run(RELEASE);
  leftMotor2.run(RELEASE);
  rightMotor1.run(RELEASE);
  rightMotor2.run(RELEASE);
}

void moveForward() {
  motorSet = "FORWARD"; // Set motor direction to forward
  leftMotor1.run(FORWARD); 
  leftMotor2.run(FORWARD); 
  rightMotor1.run(FORWARD); 
  rightMotor2.run(FORWARD);

  // Gradually increase speed to avoid quick battery drain
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) {
    leftMotor1.setSpeed(speedSet);
    leftMotor2.setSpeed(speedSet);
    rightMotor1.setSpeed(speedSet);
    rightMotor2.setSpeed(speedSet);
    delay(5);
  }
}

void moveBackward() {
  motorSet = "BACKWARD"; // Set motor direction to backward
  leftMotor1.run(BACKWARD);
  leftMotor2.run(BACKWARD);
  rightMotor1.run(BACKWARD);
  rightMotor2.run(BACKWARD);

  // Gradually increase speed
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) {
    leftMotor1.setSpeed(speedSet);
    leftMotor2.setSpeed(speedSet);
    rightMotor1.setSpeed(speedSet);
    rightMotor2.setSpeed(speedSet);
    delay(5);
  }
}

void turnRight() {
  motorSet = "RIGHT";   // Turn right
  leftMotor1.run(FORWARD);
  leftMotor2.run(FORWARD);
  rightMotor1.run(BACKWARD);
  rightMotor2.run(BACKWARD);
  rightMotor1.setSpeed(speedSet + MAX_SPEED_OFFSET);
  rightMotor2.setSpeed(speedSet + MAX_SPEED_OFFSET);
  delay(1500); // Turn right for 1500 ms

  // Go forward after turning
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);
  leftMotor2.run(FORWARD);
  rightMotor1.run(FORWARD);
  rightMotor2.run(FORWARD);
}

void turnLeft() {
  motorSet = "LEFT";    // Turn left
  leftMotor1.run(BACKWARD);
  leftMotor2.run(BACKWARD);
  rightMotor1.run(FORWARD);
  rightMotor2.run(FORWARD);
  leftMotor1.setSpeed(speedSet + MAX_SPEED_OFFSET);
  leftMotor2.setSpeed(speedSet + MAX_SPEED_OFFSET);
  delay(1500); // Turn left for 1500 ms

  // Go forward after turning
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);
  leftMotor2.run(FORWARD);
  rightMotor1.run(FORWARD);
  rightMotor2.run(FORWARD);
}

void turnAround() {
  motorSet = "TURN AROUND";  // Turn around
  leftMotor1.run(FORWARD);
  leftMotor2.run(FORWARD);
  rightMotor1.run(BACKWARD);
  rightMotor2.run(BACKWARD);
  rightMotor1.setSpeed(speedSet + MAX_SPEED_OFFSET);
  rightMotor2.setSpeed(speedSet + MAX_SPEED_OFFSET);
  delay(1700); // Turn around for 1700 ms

  // Go forward after turning
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);
  leftMotor2.run(FORWARD);
  rightMotor1.run(FORWARD);
  rightMotor2.run(FORWARD);
}
