#include "EventAnalog.h"




EventAnalog::EventAnalog(byte analogPin) {
  this->analogPin = analogPin;
  setSliceNeg();
  setSlicePos();
}
EventAnalog::EventAnalog() {
  _enabled = false;
}

void EventAnalog::setChangedHandler(EventAnalogCallback f) { changed_cb = f; }
void EventAnalog::setIdleHandler(EventAnalogCallback f) { idle_cb = f; }
int16_t EventAnalog::position() { return currentPos; }
int16_t EventAnalog::previousPosition() { return previousPos; }

void EventAnalog::update() {
  if ( _enabled || _allowRead ) {
    uint16_t readVal = analogRead(analogPin);
    if ( abs(startVal-readVal) > (startBoundary)) {
      // For joysticks, resistance either side of centre can be quite 
      // different ranges so we need to slice both sides
      if ( readVal < minVal ) {
        minVal = readVal;
        setSliceNeg();
      } else if ( readVal > maxVal ) {
        maxVal = readVal;
        setSlicePos();
      }
      if ( _enabled ) {
        if ( readVal < startVal) {
          if ( abs(readVal - previousVal) > sliceNeg ) { //Noise filtering as a bonus...
            previousVal = readVal;
            currentPos = max( (((startVal-startBoundary-readVal)*-1)/sliceNeg), negativeIncrements*-1);
          }
        } else {
          if ( abs(readVal - previousVal) > slicePos ) {
            previousVal = readVal;
            currentPos = min(((readVal-startBoundary-startVal)/slicePos), positiveIncrements);
          }
        }
      } else {
        //Always set the centrePos to ensure event fires
        currentPos = 0;
        //@TODO periodically reset startVal
      }
      if ( previousPos != currentPos ) {
        previousPos = currentPos;
        lastEventMs = millis();
        idleFired = false;
        //Serial.printf("Pos: %i \n", currentPos);
        if (changed_cb != NULL) changed_cb(*this);
      }
      //fire idle timeout callback
      if ( !idleFired && idle_cb != NULL && (millis() - lastEventMs) > idleTimeout ) {
        idleFired = true;
        idle_cb (*this);
      }
    }
  }
}

void EventAnalog::setSliceNeg() {
  sliceNeg = max(((startVal-startBoundary-minVal-endBoundary)/negativeIncrements),1); //Never allow 0
}
void EventAnalog::setSlicePos() {
  slicePos = max(((maxVal-endBoundary-startBoundary-startVal)/positiveIncrements),1);  //Never allow 0
}

void EventAnalog::setStartValue(uint16_t val) {
  startVal = max(val, 0);
  setSliceNeg();
  setSlicePos();
}

void EventAnalog::setStartBoundary(uint16_t width /*=200*/) {
  startBoundary = max(width, 1);
  setSliceNeg();
  setSlicePos();
}
void EventAnalog::setEndBoundary(uint16_t width /*=100*/) {
  endBoundary = max(width, 1);
  setSliceNeg();
  setSlicePos();
}
void EventAnalog::setNumIncrements(uint8_t numIncr /*=10*/) {
  negativeIncrements=max(numIncr, 1);
  positiveIncrements=max(numIncr, 1);
  setSliceNeg();
  setSlicePos();
}

void EventAnalog::setNumNegativeIncrements(uint8_t numIncr /*=10*/) {
  negativeIncrements=max(numIncr, 1);
  setSliceNeg();
}

void EventAnalog::setNumPositiveIncrements(uint8_t numIncr /*=10*/) {
  positiveIncrements=max(numIncr, 1);
  setSlicePos();
}

unsigned int EventAnalog::userId() { return _userId; }

unsigned int EventAnalog::userState() { return _userState; }

bool EventAnalog::enabled() { return _enabled; }

void EventAnalog::enable(bool e, bool allowRead) { 
  _enabled = e;
  _allowRead = allowRead;
}

bool EventAnalog::isIdle() {
  //Return true if idle (whether idle callback defined or not)
  return (millis() - lastEventMs) > idleTimeout;
}
