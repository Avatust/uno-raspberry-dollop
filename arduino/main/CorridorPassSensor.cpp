#include "CorridorPassSensor.h"
#include "Arduino.h"

CorridorPassSensor::CorridorPassSensor(int  sensor1_pin, int  sensor2_pin,
                                       bool sensor1_polarity, bool sensor2_polarity,
                                       eventHandler onForward, eventHandler onBackward,
                                       eventHandler onError, eventHandler onReset,
                                       unsigned long issueErrorAfter,
                                       unsigned long resetAfter
    ) :
        pinS1{sensor1_pin}, pinS2{sensor2_pin},
        polS1{sensor1_polarity}, polS2{sensor2_polarity},
        onForwardHandler{onForward},
        onBackwardHandler{onBackward},
        onErrorHandler{onError},
        onResetHandler{onReset},
        issueErrorTime{issueErrorAfter},
        resetTime{resetAfter},

        trig_ts_of_S1{0}, trig_ts_of_S2{2},
        started{false}, inErrorState{true}
{
  pinMode(pinS1, INPUT);
  pinMode(pinS2, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
}


void CorridorPassSensor::start() {
  started = true;

  bool curS1, curS2,
       lastS1 = 0,
       lastS2 = 0;

  // timestamps
  unsigned long ts1, ts2, curts;
  ts1 = ts2 = curts = millis();

  // keep watching
  while (started) {
    delay(1);
    // if in error state wait for correcting
    if (inErrorState) {
      bool LED = 0;
      unsigned long timestamp = 0;

      while (inErrorState) {
        delay(1);
        curS1 = digitalRead(pinS1) ^ polS1;
        curS2 = digitalRead(pinS2) ^ polS2;

        // blink LED
        if ( (millis() % BLINK_DELAY_ON_ERROR) == 0) {
          LED = !LED;
          digitalWrite(LED_BUILTIN, LED);
        }

        // if both are fine for some wanted time, reset
        if (!curS1 && !curS2) {
          if (!timestamp) {
            timestamp = millis();
          } else if ((timestamp + WAIT_FOR_RESET_AFTER_NO_ERROR) < millis()) {
            lastS1 = lastS2 = 0;
            ts1 = ts2 = curts = millis();
            onReset();
          }
        } else {
          timestamp = 0;
        }
      }
    } else {
      // check whether states have changed
      // state is true if it is blocked (active)
      curS1 = digitalRead(pinS1) ^ polS1;
      curS2 = digitalRead(pinS2) ^ polS2;
      curts = millis();

      // state changed?
      if ((curS1 != lastS1) && (curts > ts1 + NOISE_TIMEOUT)) {

        // if the state changed from sensor seeing something
        // to free space
        if (!curS1 && lastS1) {
          s1Triggered(curts);
        }

        // update the timestamp and state
        ts1 = curts;
        lastS1 = curS1;

      } else if (lastS1 && (curts > ts1 + issueErrorTime)) {
        onError(); // fire the error handler
      }

      // the same for S2
      // state changed?
      if ((curS2 != lastS2) && (curts > ts2 + NOISE_TIMEOUT)) {

        // if the state changed from sensor seeing something
        // to free space
        if (!curS2 && lastS2) {
          s2Triggered(curts);
        }

        // update the timestamp and state
        ts2 = curts;
        lastS2 = curS2;

      } else if (lastS2 && (curts > ts2 + issueErrorTime)) {
        onError(); // fire the error handler
      }


    }
  }
}

void CorridorPassSensor::stop() {
  started = false;
}



void CorridorPassSensor::s1Triggered(unsigned long t) {
  if (trig_ts_of_S2) {
    onBackward();
    trig_ts_of_S2 = 0;
  } else {
    trig_ts_of_S1 = 1;
  }
}

void CorridorPassSensor::s2Triggered(unsigned long t) {
  if (trig_ts_of_S1) {
    onForward();
    trig_ts_of_S1 = 0;
  } else {
    trig_ts_of_S2 = 1;
  }
}


void CorridorPassSensor::onForward (eventHandler handler) {
  onForwardHandler = handler;
}

void CorridorPassSensor::onBackward(eventHandler handler) {
  onBackwardHandler = handler;
}

void CorridorPassSensor::onError   (eventHandler handler) {
  onErrorHandler = handler;
}

void CorridorPassSensor::onReset (eventHandler handler) {
  onResetHandler = handler;
}

void CorridorPassSensor::setIssueErrorTime(unsigned long time) {
  issueErrorTime = time;
}

void CorridorPassSensor::setResetTime(unsigned long time) {
  resetTime = time;
}

void CorridorPassSensor::onForward () {
  if (onForwardHandler) // i.e. it's not null
    onForwardHandler();
}

void CorridorPassSensor::onBackward() {
  if (onBackwardHandler) // i.e. it's not null
    onBackwardHandler();
}

void CorridorPassSensor::onError   () {
  inErrorState = true;
  if (onErrorHandler) // i.e. it's not null
    onErrorHandler();
}

void CorridorPassSensor::onReset   () {
  digitalWrite(LED_BUILTIN, HIGH);
  inErrorState = false;
  trig_ts_of_S1 = 0;
  trig_ts_of_S2 = 0;
  if (onResetHandler) // i.e. it's not null
    onResetHandler();
}

