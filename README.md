# Peripheral
A simple way to handle LED, buttons, buzzer, and other I/O device.

This Arduino librairie allow you to handle simple I/O device, with all the timing problem managed by a background task.

In order to work properly, the loop function must call the function Peripherique::update(). The fastest the main loop is running,
the better will be the timing resolution. Using polling or blocking function in the main loop will cause this librairie to malfunction.
No hardware timer or interrupt are used.

This librairie is useful for projects with a big user interface (led, button ..), and when time precision is not critical.

List of class:

- Timer
- DigitalIn 
- DigitalOut
- Led 
- AnalogIn
- Buzzer
