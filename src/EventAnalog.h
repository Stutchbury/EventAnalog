/**
 * 
 * Written for Teensy but tested on Arduino Uno and others.
 * 
 * 
 */
#include "Arduino.h"


#ifndef EventAnalog_h
#define EventAnalog_h

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266) || defined(CORE_TEENSY)
  #include <functional>
#endif

class EventAnalog {

  protected:

    #if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266) || defined(CORE_TEENSY)
      typedef std::function<void(EventAnalog &btn)> EventAnalogCallback;
    #else
      typedef void (*EventAnalogCallback)(EventAnalog &);
    #endif

  public:

    /**
     * @brief Construct a new EventAnalog object
     * 
     * @param analogPin 
     */
    EventAnalog(byte analogPin);

    /**
     * @brief Construct a new Event Analog object
     * Not normally used. Will not be enabled.
     */
    EventAnalog();

    /**
     * @brief Must be called from loop() to read the analog
     * pin and fire the changed callback if required.     * 
     */
    void update();

    /**
     * @brief Read the current position - this is not the analog value
     * but the mapped position of the increments you have defined.
     * 
     * @return int16_t 
     */
    int16_t position();
    /**
     * @brief Read the previous position - this is not the analog value
     * but the mapped position of the increments you have defined.
     * 
     * @return int16_t 
     */
    int16_t previousPosition();

    /**
     * @brief Split the analog range into this number of slices.
     * A changed callback will be fire each time the increment changes. 
     * 
     * @param numIncr 
     */
    void setNumIncrements(uint8_t numIncr=10);

    /**
     * @brief Set the underlying analog value that equates to the zero position
     * 
     * @param val 
     */
    void setStartValue(uint16_t val);
    /**
     * @brief Used primarily for joysticks - it is very difficult to press the 
     * joystick button without moving the joystick so with this we can
     * create a centra 'deadzone'.
     * @param width - the analog value 
     * 
     */
    void setStartBoundary(uint16_t width=200);
    /**
     * Again, used primarily for joysticks to create an outer 'deadzone'
     * where joysticks are notoriously inconsistent.
     * @param width - the analog value 
     */
    void setEndBoundary(uint16_t width=100);

    /**
     * @brief Normally increments are set with setNumIncrements but
     * you can also set the negative and positive sides individually.
     * 
     * @param numIncr 
     */
    void setNumNegativeIncrements(uint8_t numIncr=10);
    /**
     * @brief Normally increments are set with setNumIncrements but
     * you can also set the negative and positive sides individually.
     * 
     * @param numIncr 
     */
    void setNumPositiveIncrements(uint8_t numIncr=10);

    /**
     * Fires when the analog position changes
     */
    void setChangedHandler(EventAnalogCallback f);

    /** ***************************************
     * Fired after the analog pin has been idle for 
     * setIdleTimeout(ms) (default 10 seconds)
     */
    void setIdleHandler(EventAnalogCallback f);    

/**
     * Set the idle timeout in ms (default 10000) 
     */
    void setIdleTimeout(unsigned int timeoutMs);

    /**
     * Set an identifier, ignored by library (not unique, defaults to 0)
     * Useful when calling a common handler.
     */
    void setUserId(unsigned int id);

    /**
     * Set the user state. Slight function creep but useful.
     * For use by implementor, ignored by library
     * eg ON, OFF, INACTIVE etc
     */
    void setUserState(unsigned int s);

    /**
     * Get the identifier (not unique, defaults to 0)
     */
    unsigned int userId();

    /**
     * Get the user state.
     */
    unsigned int userState();


    /**
     * Returns true if enabled
     */
    bool enabled();

    /**
     * Set enabled to true (default) or false
     * This will enable/disable all event callbacks.
     * If allowRead is set to true, will still do analogRead
     * to allow manual setting of max negative/positive values without
     * firing callbacks
     */
    void enable(bool e=true, bool allowRead=false);

    /**
     * @brief Return true if no activity for  longer than 
     * setIdleTimeout - irrespective of whether the 
     * idle (or changed) callback has been fired
     * 
     * @return true 
     * @return false 
     */
    bool isIdle();

  protected:

    EventAnalogCallback changed_cb = NULL;
    EventAnalogCallback idle_cb = NULL;
    void setSliceNeg();
    void setSlicePos();

  private:
    byte analogPin = 0;
    uint16_t startVal = 100;
    uint16_t currentVal = startVal;
    uint16_t previousVal = currentVal;
    uint16_t minVal = 100;
    uint16_t maxVal = 980;
    uint16_t startBoundary = 50;
    uint16_t endBoundary = 50;

    uint8_t negativeIncrements = 25;
    uint8_t positiveIncrements = 25;
    uint16_t sliceNeg = 10;
    uint16_t slicePos = 10;

    int16_t currentPos = 0;
    int16_t previousPos = 0;

    unsigned long lastEventMs = millis();
    unsigned long idleTimeout = 10000;
    bool idleFired = false;
    unsigned int _userId = 0;
    unsigned int _userState = 0;
    bool _allowRead = false;
    bool _enabled = true;

};



#endif

