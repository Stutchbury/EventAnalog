# EventAnalog

> **The new version of EventAnalog can now be found in the [InputEvents library](https://github.com/Stutchbury/InputEvents).**

An event based library for analog inputs - slice an analog range into configurable number of increments and a callback will be fired on each change of increment. 

For many uses of analog input, the 1024 'slices' of the analog value are more than is necessary. This library allows you to reduce those 1024 slices to a more managable number, calling a handler function or method each time the slice position increments up or down.

Start and end (or centre and edge) 'deadzones' can be configured.

This also provides effective noise reduction.

As with my other event libraries, an idle timeout callback is included.

This library follows the same principles as [EventButton](https://github.com/Stutchbury/EventButton) (but with far fewer events) and forms the basis of the [EventJoystick](https://github.com/Stutchbury/EventJoystick) library.


## INSTALLATION & DOWNLOAD
Install via the Arduino Library Manager or download from [github](https://github.com/Stutchbury/EventAnalog).

## BASIC USAGE

### INSTANTIATE

```cpp
#include <EventAnalog.h>
EventAnalog ea1(<ANALOG_PIN>);

// Create one or more callback functions 
void onEa1Changed(EventAnalog& ea) {
  Serial.print("ea1 changed. Position: ");
  Serial.println(ea.position());
}
```

### SETUP

```cpp
// Link event to function defined above
ea1.setChangedHandler(onEa1Changed);

// Optionally set the number of increments you'd like. Default is 25.
ea1.setNumIncrements(15);

```
### LOOP

```cpp
// Call 'update' for every EventAnalog
ea1.update();
```

## Constructors

Construct an EventAnalog
```cpp
EventAnalog(byte analogPin);
```

## Loop method

There is only one method to be called from loop but it must be called for each defined ```EventAnalog```.  

**```void update()```** reads the state of the analog pin and fires the appropriate handler (changed or idle).



## Setup methods

Normally called from within ```setup()``` but can be updated at runtime.

### Setting button callback handlers

Each of these methods (optionally) sets the function to be called on the event being fired. 

They take a single argument of your function name.

For an ESP8288/32 or Teensy you can also pass a class method - see ```ClassMethodLambdaCallback.ino``` in the examples.

**```setChangedHandler```** fires when the position changes.


**```setIdleHandler```** fires after the EventAnalog has been idle for ```setIdleTimeout(ms)``` (default 10 seconds).

**```setNumIncrements(uint8_t)```** Split the analog range into this number of slices. Note: This library is intended to reduce the usual 0-1024 range of ```analogRead()``` to a much smaller, more manageable number of 'slices'. Higher numbers may not produce accurate results.

A changed callback will be fired each time the increment changes. 

**```setNumNegativeIncrements(uint8_t)```** and **```setNumPositiveIncrements(uint8_t)```** Normally increments are set with ```setNumIncrements``` but you can also set the negative and positive sides individually.


**```setStartValue(uint16_t)```** Set the underlying analog value that equates to the zero position. Primarily used to set the centre for joysticks.

**```setStartBoundary(uint16_t)```** Used primarily for joysticks - it is very difficult to press the joystick button without moving the stick so with this we can create a central 'deadzone'. Parameter in the analog value, not increment position. 

**```setEndBoundary(uint16_t)```** Again, used primarily for joysticks to create an outer 'deadzone' where joysticks are notoriously inconsistent. Parameter is the analog value, not the increment position.

**```setRateLimit(uint16_t ms)```** EventAnalog callbacks are normally fired on every loop() but to limit the number of events fired when the potentiometer is moved quickly, you can set a rate limit here. Note: A high rate limit may reduce responsiveness.



### Setting id or user state 

**```setUserId(unsigned int identifier)```** Set a user defined value to identify this EventAnalog instance. Not used by the library and does not have to be unique (defaults to 0). Useful when multiple pins call the same handler.  
Note: this is 'user' as in 'user of the library'. 


**```setUserState(unsigned int state)```** Set a user defined state for this EventAnalog instance. Eg enum for ON, OFF, INACTIVE etc. Not used by the library.



### Read EventAnalog state

From within your callback function:

**```int16_t position()```**  Returns the current position - this is not the analog value but the mapped position of the increments you have defined. Can be negative if ```setStartValue```is greater than the minimum position (eg for joysticks).

**```int16_t previousPosition()```** Returns the previous position - this is not the analog value but the mapped position of the increments you have defined.

**```hasChanged()```** Returns ```true``` if position has changed since previous ```update()```.

### Timeout

**```bool isIdle()```** Returns true if no activity for  longer than ```setIdleTimeout``` - irrespective of whether the idle (or changed) callback has been fired.

### Enabling/disabling the EventAnalog

**```void enable(bool=true, allowRead=false)```** Disabling the EventAnalog will prevent all events firing. Pass ```true``` to enable (default) or ```false``` to disable.
Passing ```allowRead``` to ```true``` will allow the analogRead, enabling the 'manual' setting of max negative/positive values whilst not firing callbacks.

**```bool enabled()```** Returns ```true``` if enabled or ```false``` if disabled.



