#include "led_service.h"

led_service::led_service(float frequency, uint8_t outputPin,
                         uint32_t fadeTimeMillis)
    : timed_service(calcDutyCycleLsbInterval(frequency)) {
  ledOutputPin = outputPin;
  pinMode(ledOutputPin, OUTPUT);
  digitalWrite(ledOutputPin, LOW);
  fadeConstant = (float)DUTY_CYCLE_MAX / (float)getCallsPerMillis() /
                 (float)fadeTimeMillis;
}

led_service::~led_service() {}

uint32_t led_service::calcDutyCycleLsbInterval(float frequency) {
  return (1000 * 1000) / frequency / DUTY_CYCLE_MAX;
}

uint32_t led_service::limitDutyCycle(uint32_t dutyCycle) {
  if (dutyCycle > DUTY_CYCLE_MAX) {
    return DUTY_CYCLE_MAX;
  }

  if (dutyCycle < DUTY_CYCLE_MIN) {
    return DUTY_CYCLE_MIN;
  }

  return dutyCycle;
}

float led_service::getCalculatedFadedDutyCycle() {
  const uint32_t tmpTarget = targetDutyCycle;
  const uint32_t tmpCurrent = currentDutyCycle;

  if (tmpCurrent == tmpTarget) {
    return tmpTarget;
  }
  if (tmpCurrent < tmpTarget) {
    return currentDutyCycle + fadeConstant;
  }
  if (tmpCurrent > tmpTarget) {
    return currentDutyCycle - fadeConstant;
  }
}

float led_service::determineCurrentDutyCycle() {
  if (!shouldFade) {
    return (float)targetDutyCycle;
  } else {
    return getCalculatedFadedDutyCycle();
  }
}

float led_service::getCurrentDutyCycle() { return currentDutyCycle; }

void led_service::turnOn() {
  shouldFade = true;
  targetDutyCycle = DUTY_CYCLE_MAX;
}

void led_service::turnOff() {
  shouldFade = true;
  targetDutyCycle = DUTY_CYCLE_MIN;
}

void led_service::turnOnByDutyCycle(uint32_t dutyCycle) {
  shouldFade = false;
  targetDutyCycle = limitDutyCycle(dutyCycle);
}

void led_service::controlLed() {

  if (isTimeSlotActive()) {
    dutyCycleCounter < 100 ? dutyCycleCounter++ : dutyCycleCounter = 0;

    currentDutyCycle = determineCurrentDutyCycle();

    if (dutyCycleCounter >= (uint32_t)currentDutyCycle) {
      digitalWrite(ledOutputPin, LOW);
      return;
    }
    if (dutyCycleCounter < (uint32_t)currentDutyCycle) {
      digitalWrite(ledOutputPin, HIGH);
      return;
    }
  }
}
