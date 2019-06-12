#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

#include <Ticker.h>
// #include <asyncHTTPrequest.h>

// needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

// #include <util/dht_util.h>

// asyncHTTPrequest request;
Ticker ticker, ticker_dht;
// dht obj
// DHT dht_sensor_obj(14, DHT22);
// dht_data_t dht_data_output = {0, 0, 0};
bool dhtDataReady = false, isSendRequestTime = false;

void wifiStartConfiguration();
// void sendRequest();
// void requestCB(void *optParm, asyncHTTPrequest *request, int readyState);

void wifiStartConfiguration() {
  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it
  // around
  WiFiManager wifiManager;
  // reset settings - for testing
  // wifiManager.resetSettings();

  // sets timeout until configuration portal gets turned off
  // useful to make it all retry or go to sleep
  // in seconds
  wifiManager.setTimeout(180);

  // fetches ssid and pass and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  String sapx = String("AP_IOT_") + String(ESP.getChipId());
  if (!wifiManager.autoConnect(sapx.c_str(), "1234567890")) {
    Serial.println("Failed to connect and hit timeout. I will restart...");
    delay(3000);
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  // if you get here you have connected to the WiFi
  Serial.println("Connected...yeey :)");
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  wifiStartConfiguration();

  // request.setDebug(true);
  // ticker.attach(10, sendRequest);
}

void loop() {


  yield();
  delay(1000);
}
