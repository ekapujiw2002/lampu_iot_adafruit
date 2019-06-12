//************************************************************************************************************
//
// There are scores of ways to use asyncHTTPrequest.  The important thing to
// keep in mind is that
// it is asynchronous and just like in JavaScript, everything is event
// driven.
// You will have some
// reason to initiate an asynchronous HTTP request in your program, but then
// sending the request
// headers and payload, gathering the response headers and any payload, and
// processing
// of that response, can (and probably should) all be done asynchronously.
//
// In this example, a Ticker function is setup to fire every 5 seconds to
// initiate a request.
// Everything is handled in asyncHTTPrequest without blocking.
// The callback onReadyStateChange is made progressively and like most JS
// scripts, we look for
// readyState == 4 (complete) here.  At that time the response is retrieved
// and
// printed.
//
// Note that there is no code in loop().  A code entered into loop would run
// oblivious to
// the ongoing HTTP requests.  The Ticker could be removed and periodic calls
// to
// sendRequest()
// could be made in loop(), resulting in the same asynchronous handling.
//
// For demo purposes, debug is turned on for handling of the first request.
// These are the
// events that are being handled in asyncHTTPrequest.  They all begin with
// Debug(nnn) where
// nnn is the elapsed time in milliseconds since the transaction was started.
//
//*************************************************************************************************************
// #include <Arduino.h>
// #include <ESP8266WiFi.h>
// #include <Ticker.h>
// #include <DNSServer.h>
// #include <ESP8266WebServer.h>
// #include <WiFiManager.h>
// #include <asyncHTTPrequest.h>
//
// WiFiManager wifiManager;
// asyncHTTPrequest request;
// Ticker ticker;
//
// void sendRequest() {
//   if (request.readyState() == 0 || request.readyState() == 4) {
//     request.open("GET",
//                  "http://worldtimeapi.org/api/timezone/Asia/Jakarta.txt");
//     request.send();
//   }
// }
//
// void requestCB(void *optParm, asyncHTTPrequest *request, int readyState) {
//   if (readyState == 4) {
//     Serial.println(request->responseText());
//     Serial.println();
//     request->setDebug(false);
//   }
// }
//
// void setup() {
//   Serial.begin(57600);
//   WiFi.setAutoConnect(true);
//   WiFi.begin();
//   while (WiFi.status() != WL_CONNECTED) {
//     wifiManager.setDebugOutput(false);
//     wifiManager.setConfigPortalTimeout(180);
//     Serial.println("Connecting with WiFiManager");
//     wifiManager.autoConnect("ROBOTIC_ROOM", "Robot09876543212017");
//     yield();
//     delay(500);
//   }
//   request.setDebug(true);
//   request.onReadyStateChange(requestCB);
//   ticker.attach(10, sendRequest);
// }
//
// void loop() {}

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

#include <Ticker.h>
#include <asyncHTTPrequest.h>

// needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

#include <util/dht_util.h>

asyncHTTPrequest request;
Ticker ticker, ticker_dht;
// dht obj
DHT dht_sensor_obj(14, DHT22);
dht_data_t dht_data_output = {0, 0, 0};
bool dhtDataReady = false, isSendRequestTime = false;

void wifiStartConfiguration();
void sendRequest();
void requestCB(void *optParm, asyncHTTPrequest *request, int readyState);

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
  String sapx = String("AP_DHT_") + String(ESP.getChipId());
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

void sendRequest() {
  if (dhtDataReady) {
    if (dht_data_output.status_ok) {

      // String urlx = String("http://sister.mdp.ac.id:80/suhu/monitoring/get/")
      // +
      //               String(ESP.getChipId()) + String("/") +
      //               String(dht_data_output.temperature, 1) + String("/") +
      //               String(dht_data_output.humidity, 1);
      // // urlx = "http://worldtimeapi.org/api/timezone/Asia/Jakarta.txt";
      // Serial.println("Sending sensor data to server : " + urlx);
      // if (request.readyState() == 0 || request.readyState() == 4) {
      //   request.open("GET", urlx.c_str());
      //   request.send();
      // }

      isSendRequestTime = true;
    } else {
      Serial.println("Invalid sensor data!!!");
    }
    dhtDataReady = false;
  } else {
    Serial.println("Data sensor not ready");
  }
}

void requestCB(void *optParm, asyncHTTPrequest *request, int readyState) {
  if (readyState == 4) {
    Serial.println(request->responseText());
    Serial.println();
    // request->setDebug(false);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  wifiStartConfiguration();

  request.setDebug(true);
  request.onReadyStateChange(requestCB);
  ticker.attach(10, sendRequest);
  // ticker.attach(5, sendRequest);
}

void loop() {
  if (isSendRequestTime) {
    String urlx = String("http://sister.mdp.ac.id:80/suhu/monitoring/get/") +
                  String(ESP.getChipId()) + String("/") +
                  String(dht_data_output.temperature, 1) + String("/") +
                  String(dht_data_output.humidity, 1);
    HTTPClient http; // Declare an object of class HTTPClient

    http.setTimeout(5000);
    http.begin(urlx.c_str());
    int httpCode = http.GET();
    http.end();

    Serial.println(String(httpCode, DEC));
    isSendRequestTime = false;
  }
  if (!dhtDataReady) {
    if (get_dht_data(&dht_sensor_obj, &dht_data_output)) {
      Serial.println("Reading sensor OK");
      Serial.println(String(dht_data_output.status_ok, DEC) + "\t" +
                     String(dht_data_output.temperature, 1) + "\t" +
                     String(dht_data_output.humidity, 1));
      dhtDataReady = true;
    } else {
      Serial.println("Fail reading sensor");
    }
  }
  yield();
  delay(1000);
}
