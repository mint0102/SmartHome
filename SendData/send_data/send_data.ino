#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//Wifi name
#define WLAN_SSID       "mmm"
//Wifi password
#define WLAN_PASS       "12344321"

//setup Adafruit
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
//fill your username                   
#define AIO_USERNAME    "Mintmin"
//fill your key
#define AIO_KEY         "//"

//setup MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

//setup publish
Adafruit_MQTT_Publish light_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/log-data.light");
Adafruit_MQTT_Publish data_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/log-data.data");

//setup subcribe
// Adafruit_MQTT_Subscribe light_sub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/log-data.data", MQTT_QOS_1);

int led_counter = 0;
int led_status = HIGH;
int check_load = 0;
int check_motion = 0;

// void lightcallback(char* value, uint16_t len){
//   if(value[0] == '0') Serial.print('a');
//   if(value[0] == '1') Serial.print('A');
// }

void setup() {
  // put your setup code here, to run once:
  //set pin 2,5 as OUTPUT
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  //set busy pin HIGH
  digitalWrite(5, HIGH);

  Serial.begin(115200);

  //connect Wifi
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  //subscribe light feed
  // light_sub.setCallback(lightcallback);
  // mqtt.subscribe(&light_sub);
  // mqtt.subscribe(&data_sub);

  //connect MQTT
  while (mqtt.connect() != 0) { 
    mqtt.disconnect();
    delay(500);
  }

  //finish setup, set busy pin LOW
  digitalWrite(5, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  //receive packet
  // mqtt.processPackets(1);
  
  //read serial
  if(Serial.available()){
    uint8_t msg = Serial.read();
    if(msg == 'o') Serial.print('O');
    else{
      switch (msg) {
      case 'l':
        if(check_load == 0){
          data_pub.publish("Button 1 is loading...");
          delay(500);
          data_pub.publish("Button 2 is loading...");
          delay(500);
          data_pub.publish("Device C1 is loading...");
          delay(500);
          data_pub.publish("Device C2 is loading...");
          delay(500);
          check_load = 1;
        }
        break;
      case 'u':
        data_pub.publish("Button 1 is ON");
        check_load = 0;
        break;
      case 'U':
        data_pub.publish("Button 1 is OFF.");
        check_load = 0;
        break;
      case 'v':
        data_pub.publish("Button 2 is ON");
        check_load = 0;
        break;
      case 'V':
        data_pub.publish("Button 2 is OFF.");
        check_load = 0;
        break;
      case 'm':
        data_pub.publish("Device C1 is ON");
        check_load = 0;
        break;
      case 'M':
        data_pub.publish("Device C1 is OFF.");
        check_load = 0;
        break;
      case 'n':
        data_pub.publish("Device C2 is ON");
        check_load = 0;
        break;
      case 'N':
        data_pub.publish("Device C2 is OFF.");
        check_load = 0;
        break;
      case 's':
        if(check_motion >= 2){
          data_pub.publish("Motion sensor is scanning.");
          check_motion = 0;
          delay(500);
        }
        else
          check_motion++;
        check_load = 0;
        break;
      }
    }
  }

  led_counter++;
  if(led_counter == 100){
    // every 1s
    led_counter = 0;
    //toggle LED
    if(led_status == HIGH) led_status = LOW;
    else led_status = HIGH;

    digitalWrite(2, led_status);
  }
  delay(10);
}
