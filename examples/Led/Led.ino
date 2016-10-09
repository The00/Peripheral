#include <peripherique.h>


Led led(13); // declare a led object attached to pin 13.

void setup() {
  
led.blink(1500,5); // setup the led to blink 5 time with a 1500ms period.
}

void loop() {
 Peripherique::update();
 if(led.getState()== 0) // wait the led to finish blinking
 {
   led.blink(100); // blink indefinitely with a 100ms period.
 }   
}
