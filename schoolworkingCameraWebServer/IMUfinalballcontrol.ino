#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MPU6050.h>
// Define software serial pins for HC
#define txPin 7
#define rxPin 6

SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);

// Define the pins for the motor driver
int MotorIA1 = 9;
int MotorIA2 = 5;
int MotorIB1 = 10;
int MotorIB2 = 11;
int fast = 160;
int slow = 60;
int stop = 0;
MPU6050 mpu;

int accelerationStep = 5;  // Adjust this value for smoother acceleration
int delayTime = 25;        // Adjust this value for the speed of acceleration

void setup() {
  // Set the motor control pins as outputs
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  pinMode(MotorIA1, OUTPUT);
  pinMode(MotorIA2, OUTPUT);
  pinMode(MotorIB1, OUTPUT);
  pinMode(MotorIB2, OUTPUT);

  // Start the serial communication
  Serial.begin(9600);
  // Set the baud rate for the SoftwareSerial object
  mySerial.begin(9600);
  // send an intro:
  mySerial.println("Program is already to Run");
  mySerial.println();

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
  }

  mySerial.println("Controller Ready");
}

void loop() {
  // Check if data is available to read from the serial port
  if (mySerial.available() > 0) {
    // Read the incoming character
    char input = mySerial.read();

    // Use a switch statement to handle different inputs
    switch (input) {
      case 'A':
      case 'a':
        // Rotate both motors forward with gradual speed increase
        forward();
        //delay(500);
        stopMotors();
        break;
      case 'B':
      case 'b':
        // Rotate both motors backward for 500 milliseconds
        backward();
        //delay(500);
        stopMotors();
        break;
      case 'C':
      case 'c':
        // Turn right for 500 milliseconds
        turnRight();
        delay(500);
        stopMotors();
        break;
      case 'D':
      case 'd':
        // Turn left for 500 milliseconds
        turnLeft();
        delay(500);
        stopMotors();
        break;
      case 'F':
      case 'f':

        break;
      case 'E':
      case 'e':
      default:
        // Handle any other input
        for (int speed = fast; speed >= 0; speed -= accelerationStep) {
          analogWrite(MotorIA1, speed);
          analogWrite(MotorIA2, stop);
          analogWrite(MotorIB1, speed);
          analogWrite(MotorIB2, stop);
          delay(delayTime);
        }

        stopMotors();
        Serial.println("Uc");

        // Read the IMU6050 data
        int16_t ax, ay, az;
        int16_t gx, gy, gz;
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

        // Calculate the angle or balance position
        float angle = atan2(ay, az) * 180 / PI;

        // Adjust motor speeds based on the angle
        if (angle > 5) {
          // Tilted forward
          ajustbackward();
          delay(300);
        } else if (angle < -5) {
          // Tilted backward
          ajustforward();
          delay(300);
        } else {
          stopMotors();
        }
        break;
    }
  }
}

// Function to rotate both motors forward with gradual speed increase
void forward() {
  // Set the direction of the motors
  for (int speed = 0; speed <= fast; speed += 10) {
    analogWrite(MotorIA1, speed);
    analogWrite(MotorIA2, stop);
    analogWrite(MotorIB1, speed);
    analogWrite(MotorIB2, stop);
    delay(delayTime);
  }
  delay(100);
  /*analogWrite(MotorIA1, fast);
  analogWrite(MotorIA2, stop);
  analogWrite(MotorIB1, fast);
  analogWrite(MotorIB2, stop);*/
}


// Function to rotate both motors backward
void backward() {
  // Set the direction of the motors to backward
  for (int speed = 0; speed <= fast; speed += 10) {
    analogWrite(MotorIA1, stop);
    analogWrite(MotorIA2, speed);
    analogWrite(MotorIB1, stop);
    analogWrite(MotorIB2, speed);
    delay(delayTime);
  }
  delay(100);
  /*analogWrite(MotorIA1, stop);
  analogWrite(MotorIA2, fast);
  analogWrite(MotorIB1, stop);
  analogWrite(MotorIB2, fast);*/
}

// Function to turn right
void turnRight() {
  // Set the direction of the motors to turn right
  /*analogWrite(MotorIA1, fast);
  analogWrite(MotorIA2, stop);
  analogWrite(MotorIB1, stop);
  analogWrite(MotorIB2, fast);*/
  digitalWrite(MotorIA1, HIGH);
  digitalWrite(MotorIA2, LOW);
  digitalWrite(MotorIB1, LOW);
  digitalWrite(MotorIB2, HIGH);
}

// Function to turn left
void turnLeft() {
  // Set the direction of the motors to turn left
  /*analogWrite(MotorIA1, stop);
  analogWrite(MotorIA2, fast);
  analogWrite(MotorIB1, fast);
  analogWrite(MotorIB2, stop);*/
  digitalWrite(MotorIA1, LOW);
  digitalWrite(MotorIA2, HIGH);
  digitalWrite(MotorIB1, HIGH);
  digitalWrite(MotorIB2, LOW);
}

// Function to stop both motors
void stopMotors() {
  digitalWrite(MotorIA1, LOW);
  digitalWrite(MotorIA2, LOW);
  digitalWrite(MotorIB1, LOW);
  digitalWrite(MotorIB2, LOW);
}

void ajustforward() {
  // Set the direction of the motors
  for (int speed = 0; speed <= slow; speed += accelerationStep) {
    analogWrite(MotorIA1, speed);
    analogWrite(MotorIA2, stop);
    analogWrite(MotorIB1, speed);
    analogWrite(MotorIB2, stop);
    delay(delayTime);
  }
}

void ajustbackward() {
  // Set the direction of the motors to backward
  for (int speed = 0; speed <= fast; speed += accelerationStep) {
    analogWrite(MotorIA1, slow);
    analogWrite(MotorIA2, speed);
    analogWrite(MotorIB1, stop);
    analogWrite(MotorIB2, speed);
    delay(delayTime);
  }
}