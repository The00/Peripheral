#include <peripherique.h>

DigitalIn button(7,100); // create a button object attached to pin 7 
                         // and sampled with a 100ms period
                         
 Led led(13); // create a led object attached to pin 13.  


void isr()
{
  led.blink(200,15); // the isr make the led blink 15 time
  }

void setup() {
  
button.startInterrupt(isr, FALLING, 3); // start the interrupt with a 3 bounce spacing
}

void loop() {
  
  Peripherique::update();

}
