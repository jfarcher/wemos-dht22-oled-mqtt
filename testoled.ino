#include "config.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SimpleDHT.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


WiFiClient ESPClient;
PubSubClient client(ESPClient);

int pinDHT11 = 2;
SimpleDHT22 dht11;


// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
 
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
 
 
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
 
 
void setup()   {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  #define mac  WiFi.macAddress()
  #define mqtt_h_topic (String("device/" + mac + "/humidity").c_str())
  #define mqtt_t_topic (String("device/" + mac + "/temperature").c_str())
  
  delay(10);
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done
  Serial.println (mac);
  display.display();
  display.clearDisplay();
  delay(2000);
  display.setTextSize(1);
 
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setCursor(32,8);
  delay(1500);
  display.clearDisplay();
  
}
void setup_wifi() {  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart(); 
  }
  //Serial.println(WiFi.localIP());

} 

void reconnect() {
  while (!client.connected()){
    Serial.println("Attempting MQTT connection.....");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");      


    } else{
      Serial.println("Failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}
long lastMsg = 0;
float temperature = 0.0; 
void loop() {
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
display.setTextSize(1);
display.setCursor(32,8);
display.println("Temp:");
display.setCursor(32,16);
display.setTextSize(2);
display.println((int)temperature);
display.display();
delay(2000);
display.clearDisplay();
// DHT11 sampling rate is 1HZ.
delay(1500);
display.setTextSize(1);
display.setCursor(32,8);
display.println("Hum:");
display.setCursor(32,16);
display.setTextSize(2);
display.println((int)humidity);
display.display();
delay(2000);
display.clearDisplay();
// DHT11 sampling rate is 1HZ.
delay(1500);
  
   long now = millis();
  if (now - lastMsg > 1000){    
    lastMsg = now;
    if (!client.connected()) {
      reconnect();
    }
    client.publish(mqtt_t_topic, String(temperature).c_str(), false);
    client.publish(mqtt_h_topic, String(humidity).c_str(), false);
  }
  client.loop();
}
 


