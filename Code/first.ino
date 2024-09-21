#include <Servo.h>
#include <Wire.h>

const int MPU_addr = 0x68;  // Change to 0x69 if AD0 is connected to VCC

int16_t gyroX, gyroY;
float Ang_X, Ang_Y;
Servo servo_1, servo_2;
float offset_X, offset_Y;
float errX, errY;
float coeff = 0.5; 

void MPU_START() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // Power Management 1
  Wire.write(0);    // Wake up the MPU6050
  Wire.endTransmission(true);
  Serial.println("Connection Start!");
}

Servo servo_init(int pin) {
  Servo result;
  result.attach(pin);
  return result;
}

void setup() {
  Serial.begin(19200);
  Wire.begin();
  MPU_START();
  servo_1 = servo_init(7);
  servo_2 = servo_init(6);
  Ang_X = 0;
  Ang_Y = 0;
  offset_X = 15;
  offset_Y = -5;
}

void loop() {
  // Request 4 bytes of data starting from register 0x43 (Gyroscope X)
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // Starting from Accelerometer register
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 4, true);

  // Read the gyroscope values
  gyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  gyroY = (Wire.read() << 8 | Wire.read()) / 131.0;

  // Print the raw gyro values
  Serial.print("Raw gyroX: ");
  Serial.print(gyroX);
  Serial.print("\tRaw gyroY: ");
  Serial.println(gyroY);

  // Check for serial input
  if (Serial.available() > 0) {
    Ang_X = Serial.parseInt();
    Ang_Y = Serial.parseInt();
    Serial.read(); // Consume any remaining characters (usually newline)
    
    Serial.print("Ang_X: ");
    Serial.print(Ang_X);
    Serial.print(" Ang_Y: ");
    Ang_Y *= -1;
    Serial.println(Ang_Y);

  }

  float servoPos1 = gyroX * coeff  + offset_X;
  float servoPos2 = gyroY * coeff + offset_Y;
  servoPos1 = Ang_X - servoPos1;
  servoPos2 = Ang_Y - servoPos2;
  // Scale or map the gyro values to the servo position (0 to 180 degrees)
  servoPos1 = map(servoPos1, -90, 90, 0, 180);
  servoPos2 = map(servoPos2, -90, 90, 0, 180);

  // Constrain the positions to be within the servo's valid range
  servoPos1 = constrain(servoPos1, 0, 180);
  servoPos2 = constrain(servoPos2, 0, 180);

  // Set the servo positions
  servo_1.write(servoPos1);
  servo_2.write(servoPos2);

  // Print the servo positions
  Serial.print("Servo 1 position: ");
  Serial.print(Ang_X-servoPos1);
  Serial.print(" Servo 2 position: ");
  Serial.println(Ang_Y-servoPos2);

  delay(50); // Small delay to make the output readable
}
