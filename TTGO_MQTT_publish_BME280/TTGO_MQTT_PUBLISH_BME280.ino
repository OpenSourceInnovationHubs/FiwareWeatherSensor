/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-lora-sensor-web-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/
//TTGO LoRa32 SX1276 OLED

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Libraries for BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//Libraries for Web connection
#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//BME280 definition
#define SDA 21
#define SCL 13

TwoWire I2Cone = TwoWire(1);
Adafruit_BME280 bme;

const char* SSID = "<YOUR-WIFI-CONNECTION>";
const char* WiFiPassword = "<YOUR-WIFI-PASSWORD>";
 
const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic[]  = "/fhtw/368415464/9hids/we2j332";

WiFiClient espClient;
MqttClient mqttClient(espClient);

//packet counter
int readingID = 0;

int counter = 0;

float temperature = 0;
float humidity = 0;
float pressure = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void ConnectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WiFiPassword);
  Serial.print("Connecting to "); Serial.println(SSID);
 
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
 
    if ((++i % 16) == 0)
    {
      //Serial.println(F(" still trying to connect"));
      Serial.println(F("Restart"));
      delay(500);
      ESP.restart();
    }
  }
 
  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
 
  if(WiFi.status() == WL_CONNECTED) {
    for (i=0;i<4;i++) {
      digitalWrite(BUILTIN_LED,LOW);
      delay(100);
      digitalWrite(BUILTIN_LED,HIGH);
      delay(100);
      Serial.print(F("blink "));
      digitalWrite(BUILTIN_LED, LOW);
      }
  }
}
 
void ConnectToMQTT() {
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
 
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }
 
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

//Initialize OLED display
void startOLED(){
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER");
}

void startBME(){
  I2Cone.begin(SDA, SCL, 100000); 
  bool status1 = bme.begin(0x76, &I2Cone);  
  if (!status1) {
    Serial.println("Could not find a valid BME280_1 sensor, check wiring!");
    while (1);
  }
}

void getReadings(){
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
}

void sendDataMQTT(){
  StaticJsonDocument<48> doc;
  char output[128];
  doc["id"] = WiFi.macAddress();
  doc["temp"] = temperature;
  doc["hum"] = humidity;
  doc["pressure"] = pressure;

  serializeJson(doc, output);

  //float value = 50.3;
  mqttClient.beginMessage(topic);
  mqttClient.print(output);
  mqttClient.endMessage();
  Serial.print(" data sent: ");
  Serial.print(readingID);
  Serial.println("");
}

void sendReadings() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);

  display.setCursor(0,20);
  display.print("Temperature:");
  display.setCursor(75,20);
  display.print(temperature);
  Serial.println("");
  Serial.print(temperature);
  
  display.setCursor(0,30);
  display.print("Humidity:");
  display.setCursor(75,30);
  display.print(humidity);
  Serial.print(", ");
  Serial.print(humidity);
  
  display.setCursor(0,40);
  display.print("Pressure:");
  display.setCursor(75,40);
  display.print(pressure);
  Serial.print(", ");
  Serial.print(pressure);
  
  //display.setCursor(0,50);
  //display.print("Reading ID:");
  //display.setCursor(66,50);
  //display.print(readingID);
  display.display();

  //Serial.println(readingID);
  readingID++;

  sendDataMQTT();
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  startOLED();
  startBME();
  ConnectToWiFi();
  ConnectToMQTT();
  Serial.println("setup done");
}

void loop() {
  getReadings();
  sendReadings();
  delay(5000);
}
