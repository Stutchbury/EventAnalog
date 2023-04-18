/**
 *
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2023 Philip Fletcher <philip.fletcher@stutchbury.com> 
 * 
 */


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
bool EventAnalog::hasChanged() { return _hasChanged; }

void EventAnalog::update() {
  if (!_started) {
    // Set the start position so we don't trigger an event before moving
    readVal = analogRead(analogPin);
    setReadPos(readVal - startVal);
    currentPos = readPos;
    previousPos = currentPos;
    _started = true;
  }


  if ( _enabled || _allowRead ) {
    _hasChanged = false;
    readVal = analogRead(analogPin);
    // For joysticks, resistance either side of centre can be quite 
    // different ranges so we need to slice both sides
    if ( readVal < minVal ) {
      minVal = readVal;
      setSliceNeg();
    } else if ( readVal > maxVal ) {
      maxVal = readVal;
      setSlicePos();
    }

    if ( _enabled && millis() > (rateLimitCounter + rateLimit) ) { 
      setReadPos(readVal - startVal);
      if ( currentPos != readPos ) {
        previousPos = currentPos;
        currentPos = readPos;
        lastEventMs = millis();
        idleFired = false;
        _hasChanged = true;
        if (changed_cb != NULL) changed_cb(*this);
      }
      //fire idle timeout callback
      if ( !idleFired && idle_cb != NULL && (millis() - lastEventMs) > idleTimeout ) {
        idleFired = true;
        idle_cb (*this);
      }
      rateLimitCounter = millis();
    }
  }
}

void EventAnalog::setReadPos(int16_t offset) {
  if ( offset > startBoundary) { //Going up!
    if ( abs(readVal - previousVal) > slicePos ) {
      previousVal = readVal;
      int16_t rawReadPos = ((readVal-startBoundary-startVal)/slicePos);
      readPos = min(rawReadPos, (positiveIncrements));
    }
  } else if (abs(offset) > startBoundary) { //Going down
    if ( abs(readVal - previousVal) > sliceNeg ) {
      previousVal = readVal;
      int16_t rawReadPos = ((startVal-startBoundary-readVal)/sliceNeg) * -1;
      readPos = max(rawReadPos, (negativeIncrements*-1));
    }
  } else {
    previousVal = readVal;
    readPos = 0;
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

void EventAnalog::setIdleTimeout(unsigned int timeoutMs) {
  idleTimeout = timeoutMs;
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

void EventAnalog::setRateLimit(uint16_t ms) { rateLimit = ms; }

