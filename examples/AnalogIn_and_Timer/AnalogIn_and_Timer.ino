#include <peripherique.h>



AnalogIn adc(0,200,1,1); // create a analog input object
                       // attached to A0, and sampled at 200ms.
                       // the conversion value is set to one, so the result will be in volt
                       // the moving average buffer is set to 16s

void callb()
{
  Serial.println(adc.getVal());  
}
  

Timer tim1(1000, callb, 1); // create a timer that will call callb every second.
                               // last param = 1 => the timer start immediately
                               // last param = 0 => the timer must be started with start()
void setup() {
  Serial.begin(9600);

}

void loop() {
  Peripherique::update();

}
