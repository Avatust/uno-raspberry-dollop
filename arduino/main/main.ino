#include "CorridorPassSensor.h"

#define PIN_OF_SENSOR_1 4
#define PIN_OF_SENSOR_2 12
#define POLARITY 0

// handlers
void onForwardHandler();
void onBackwardHandler();
void onErrorHandler();
void onResetHandler();

void setup() {

  Serial.begin(9600);
  CorridorPassSensor s(PIN_OF_SENSOR_1, PIN_OF_SENSOR_2,
      POLARITY, POLARITY,
      onForwardHandler, onBackwardHandler,
      onErrorHandler, onResetHandler);

  // will block the program
  s.start();

}

void loop() {
  // does not get executed if sensing was started
}


void onForwardHandler() {
  Serial.print('I'); // 'I' as in/inside
}

void onBackwardHandler() {
  Serial.print('O'); // Outside
}

void onErrorHandler() {
  Serial.print('E'); // Error
}

void onResetHandler() {
  Serial.print('R'); // Reset
}
