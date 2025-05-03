#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin(); // default SDA = 21, SCL = 22

  Serial.println("Initializing MPU6050...");
  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful!");
  } else {
    Serial.println("MPU6050 connection failed.");
    while (1); // Stop execution
  }
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  // Read accelerometer and gyroscope values
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Convert raw accelerometer values to 'g' units
  float ax_g = ax / 16384.0;
  float ay_g = ay / 16384.0;
  float az_g = az / 16384.0;

  // Calculate total acceleration magnitude
  float total_acc = sqrt(ax_g * ax_g + ay_g * ay_g + az_g * az_g);

  // Calculate dip angle in degrees
  float dip_rad = acos(az_g / total_acc);  // Assuming Z-axis is up
  float dip_angle = dip_rad * 180.0 / PI;

  // Mock magnetometer heading (e.g., X-axis points 70° from North)
  float heading = 70.0;

  // Dip direction = heading + 90° (mod 360)
  float dip_direction = fmod((heading + 90.0), 360.0);

  // Strike = dip_direction - 90° (mod 360)
  float strike = fmod((dip_direction + 270.0), 360.0);

  // Print all data
  Serial.print("Accel (g): X=");
  Serial.print(ax_g, 2);
  Serial.print(" Y=");
  Serial.print(ay_g, 2);
  Serial.print(" Z=");
  Serial.print(az_g, 2);

  Serial.print(" | Dip Angle: ");
  Serial.print(dip_angle, 2);
  Serial.print("°");

  Serial.print(" | Dip Direction: ");
  Serial.print(dip_direction, 2);
  Serial.print("°");

  Serial.print(" | Strike: ");
  Serial.print(strike, 2);
  Serial.println("°");

  delay(1000); // Print once every second
}
