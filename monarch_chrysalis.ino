#include <SPI.h>  
#include <Wire.h>  
#include <Adafruit_GFX.h>  
#include <Adafruit_SSD1306.h>  
#include "DHT.h"
#include <TinyStepper_28BYJ_48.h>
//   for DHT11,   
//   VCC: 5V or 3V  
//   GND: GND  
//   DATA: 3  
#define DHTPIN 3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int soundAPin = A0;
int soundDPin = 4;
int val = 0;
float tempDiff = 0;
float humidDiff = 0;
float oldTemp = 0;
float oldHumid = 0;
int speed = 256;
int acc = 512;
const int MOTOR_IN1_PIN = 5;
const int MOTOR_IN2_PIN = 6;
const int MOTOR_IN3_PIN = 11;
const int MOTOR_IN4_PIN = 10;
const int STEPS_PER_REVOLUTION = 2048;
TinyStepper_28BYJ_48 stepper;
#define screen_width 128 // OLED display width, in pixels  
#define screen_height 64 // OLED display height, in pixels  
#define OLED_RESET 4  
Adafruit_SSD1306 display(screen_width, screen_height);  
void setup ()  {
  Serial.begin(9600);  
  Serial.println("TEMPERATURE AND HUMIDITY");
  dht.begin();
  pinMode(soundAPin, INPUT);
  pinMode(soundDPin, INPUT);  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.clearDisplay();
  stepper.connectToPins(MOTOR_IN1_PIN, MOTOR_IN2_PIN, MOTOR_IN3_PIN, MOTOR_IN4_PIN);
  delay(10);
  oldTemp = dht.readTemperature();
  Serial.print("Old Temperature: ");
  Serial.println(oldTemp);
  oldHumid = dht.readHumidity();
  Serial.print("Old Humid: ");
  Serial.println(oldHumid);
}  
void loop ()  {
  val = analogRead(soundAPin);
  Serial.print("Sound Level: ");
  Serial.println(val);
  delay(10);
  float temperature = dht.readTemperature(); 
  Serial.print("Current Temperature: ");
  Serial.println(temperature); 
  float humidity = dht.readHumidity();
  float f = dht.readTemperature(true);
  if (isnan(temperature) || isnan(humidity) || isnan(f)) {  
   Serial.print("Read DHT11 failed");delay(100);  
   return;  
  }  
  Serial.print((temperature*1.8)+32); Serial.print(" *F, ");   
  Serial.print(humidity); Serial.println(" H");  
  // DHT11 sampling rate is 1HZ.  
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);  
  display.setTextSize(1);  
  display.setCursor(0, 0);  
  display.print("       EXO-EXO");  
  display.setCursor(0, 20);  
  display.print(" TEMPERATURE:");  
  display.setCursor(86, 20);  
  display.print((temperature*1.8)+32);
  display.print(" F");  
  display.setCursor(0, 40);  
  display.print(" HUMIDITY:");  
  display.setCursor(86, 40);  
  display.print(humidity);
  display.print(" %");
  display.drawRect(0, 56, 128, 8, WHITE);
  display.fillRect(0, 56, val-460, 8, WHITE);
  display.display();
  delay(10);
  tempDiff = temperature - oldTemp;
  Serial.print("Temperature Difference: ");
  Serial.println(tempDiff);
  speed = speed + (10*tempDiff);
  humidDiff = humidity - oldHumid;
  Serial.print("Humid Difference: ");
  Serial.println(humidDiff);
  acc = acc + (int)(5*humidDiff);

  stepper.setSpeedInStepsPerSecond(speed);
  stepper.setAccelerationInStepsPerSecondPerSecond(acc);
  Serial.print("Speed: ");
  Serial.println(speed);

  stepper.moveRelativeInSteps(2048);
  Serial.println("motor moving forward");
  stepper.moveRelativeInSteps(-2048);
  Serial.println("motor moving back");
  delay(100);

  oldTemp = temperature;
  oldHumid = humidity;
}