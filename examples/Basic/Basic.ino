/**
 * A basic example of using the EventAnalog library.
 */
#include <EventAnalog.h>

/**
 * Instantiate an EventAnalog.
 * Pin must be analog capable.
 */
EventAnalog ea1(A0);


/**
 * A function to handle the 'changed' event
 * Can be called anything but requires EventAnalog& 
 * as its only parameter.
 */
void onEa1Changed(EventAnalog& eb) {
  Serial.print("ea1 position: ");
  Serial.println(eb.position());
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(500);
  Serial.println("EventAnalog Basic Example");

  ea1.setNumIncrements(15); //Optional. Default is 25 
  //Link the changed event to your function
  ea1.setChangedHandler(onEa1Changed);
   
}

void loop() {
  // put your main code here, to run repeatedly:
  // You must call update() for every defined EventAnalog.
  // This will read the state of the analog pin and 
  // fire the changed events if appropriate.
  ea1.update();
}
