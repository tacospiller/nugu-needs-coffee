#include "secrets.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#define AWS_IOT_SUBSCRIBE_TOPIC   "coffee/start"
#define AWS_IOT_PUBLISH_TOPIC   "coffee/done"


WiFiClientSecure net = WiFiClientSecure();
PubSubClient mqtt = PubSubClient();

time_t relayClosedAt;

void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void setup()
{
    Serial.begin(115200);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    Serial.print("\n");

    net.setTrustAnchors(new X509List(AWS_CERT_CA));
    net.setClientRSACert(new X509List(AWS_CERT_CRT), new PrivateKey(AWS_CERT_PRIVATE));
    setClock();

    mqtt.setServer(AWS_IOT_ENDPOINT, 8883);
    mqtt.setClient(net);
    mqtt.setCallback(callback);
    
    Serial.print("Connecting to AWS IOT");
    
    while (!mqtt.connect(THINGNAME)) {
        Serial.printf(".");
        delay(100);
    }

    Serial.print("\n");

    if(!mqtt.connected()){
        Serial.println("AWS IoT Timeout!");
        return;
    }

    relayClosedAt = 0;
    pinMode(5, OUTPUT);  
    mqtt.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

    Serial.println("AWS IoT Connected!");
}

void loop()
{
    mqtt.loop();
    relayLoop();
    delay(1000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  closeRelay();
}

void closeRelay() {
    if (relayClosedAt == 0) {
        relayClosedAt = time(nullptr);
    }
}

void relayLoop() {
    if (relayClosedAt != 0) {
        if (difftime(time(nullptr), relayClosedAt) <= 60 * 5) {
            digitalWrite(5, HIGH);
        } else {
            relayClosedAt = 0;
            digitalWrite(5, LOW);

            mqtt.publish(AWS_IOT_PUBLISH_TOPIC, AWS_IOT_PUBLISH_TOPIC);
        }
    } else {
        digitalWrite(5, LOW);
    }
}