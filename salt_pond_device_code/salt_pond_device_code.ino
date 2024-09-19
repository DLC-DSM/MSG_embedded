#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

void setup() {
  const char* ssid = "name";     
  const char* password="password"; 
  const char* serverAddress = "http://255.255.0.0:8080/plant_m/connect"; 
  // Serial init
  Serial.begin(115200);
  // Wire init
  Wire.begin();
  // Power Management
  /*Wire.beginTransmission(0x68);
  Wire.write(107);
  Wire.write(0);
  Wire.endTransmission();*/
  // Register 27
  Wire.beginTransmission(0x68);
  Wire.write(27);
  Wire.write(0);
  Wire.endTransmission();
  // Register 28
  Wire.beginTransmission(0x68);
  Wire.write(28);
  Wire.write(0);
  Wire.endTransmission();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
}

void loop() {
  uint8_t i;
  int16_t acc_raw[3], gyro_raw[3];
  // Get Accel
  Wire.beginTransmission(0x68);
  Wire.write(59);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 6);
  for(i = 0; i < 3; i++) acc_raw[i] = (Wire.read() << 8) | Wire.read();
  // Get Gyro
  Wire.beginTransmission(0x68);
  Wire.write(67);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 6);
  for(i = 0; i < 3; i++) gyro_raw[i] = (Wire.read() << 8) | Wire.read();
  // Serial print
  if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      WiFiClient client;
      http.begin(client, serverAddress); 
      http.addHeader("Content-Type", "application/json");
      String jsonData = "{\"AccX\":"+String(acc_raw[0])+",\"AccY\":"+String(acc_raw[1])+",\"AccZ\":"+String(acc_raw[2])+",\"GyroX\":"+String(gyro_raw[0])+",\"GyroY\":"+String(gyro_raw[1])+",\"GyroZ\":"+String(gyro_raw[2])+"}"; 
      int httpResponseCode = http.POST(jsonData);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("HTTP Response Code: " + String(httpResponseCode));
        Serial.println(response);
      } else {
        Serial.println("Error in HTTP  request");
      }

      http.end();
  }
}