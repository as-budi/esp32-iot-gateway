#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
 
#define AWS_IOT_PUBLISH_TOPIC   "irapid_ble"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
 
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}
 
void publishMessage()
{
  StaticJsonDocument<512> doc;
  //DynamicJsonDocument doc(1024);
  doc["timestamp"] = millis();
  doc["deviceID"] = "1099";
  doc["bleAddress"] = "f2:ab:73:19:59:79";
  doc["proximityUUID"] = "B9407F30-F5F8-466E-AFF9-25556B57FE6D";
  doc["rssi"] = -31;
  doc["txPower"] = -55;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  Serial.println("publish");
  Serial.println(jsonBuffer);
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

//  StaticJsonDocument<200> doc;
//  deserializeJson(doc, payload);
//  const char* message = doc["message"];
}
//void messageHandler(char* topic, byte* payload, unsigned int length)
//{
//  Serial.print("incoming: ");
//  Serial.println(topic);
// 
//  StaticJsonDocument<200> doc;
//  deserializeJson(doc, payload);
//  const char* message = doc["message"];
//  Serial.println(message);
//}
 
void setup()
{
  Serial.begin(115200);
  connectAWS();
}
 
void loop()
{ 
  publishMessage();
  client.loop();
  delay(1000);
}
