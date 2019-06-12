#include "adafruit_config.h"
#include <Arduino.h>
#include <Ticker.h>

// digital pin 5
#define LED_PIN LED_BUILTIN

#define LAMPU1 5
#define LAMPU2 4
#define SW1 0
#define SW2 2

// set up the 'lampu1' feed
AdafruitIO_Feed *lampu1 = io.feed("lampu1"), *lampu2 = io.feed("lampu2"),
                *lampu1_sts = io.feed("lampu1-sts"),
                *lampu2_sts = io.feed("lampu2-sts");

Ticker ticker_status;
bool send_status = false;

void handleMessage(AdafruitIO_Data *data);
void send_lampu_status();

void send_lampu_status() { send_status = true; }

void setup() {

  // init gpio
  pinMode(LED_PIN, OUTPUT);
  pinMode(LAMPU1, OUTPUT);
  pinMode(LAMPU2, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);

  // start the serial connection
  Serial.begin(57600);

  // wait for serial monitor to open
  while (!Serial)
    ;

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler
  lampu1->onMessage(handleMessage);
  lampu2->onMessage(handleMessage);

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  // get status
  lampu1->get();
  lampu2->get();

  ticker_status.attach(5, send_lampu_status);
}

void loop() {
  io.run();

  if (send_status) {
    lampu1_sts->save(digitalRead(LAMPU1));
    lampu2_sts->save(digitalRead(LAMPU2));
    send_status = false;
  }

  if (!digitalRead(SW1)) {
    while (!digitalRead(SW1)) {
      io.run();
      yield();
      delay(10);
    }
    digitalWrite(LAMPU1, !digitalRead(LAMPU1));
  }

  if (!digitalRead(SW2)) {
    while (!digitalRead(SW2)) {
      io.run();
      yield();
      delay(10);
    }
    digitalWrite(LAMPU2, !digitalRead(LAMPU2));
  }
}

// this function is called whenever an 'lampu1' feed message
// is received from Adafruit IO. it was attached to
// the 'lampu1' feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {

  Serial.print("received <- ");

  String fnx = String(data->feedName());
  Serial.print(fnx);
  Serial.print(" ");
  Serial.println(data->value());

  if (fnx == "lampu1") {
    digitalWrite(LAMPU1, atoi(data->value()));
  }

  if (fnx == "lampu2") {
    digitalWrite(LAMPU2, atoi(data->value()));
  }
}
