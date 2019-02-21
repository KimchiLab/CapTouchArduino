# CapTouchArduino
Capacitive Touch translation circuit for Arduino and MPR121 Sensor

The Arduino monitors an MPR121 for changes in capacitance and essentially copies the state of MPR121 inputs to Arduino digital outputs

Various versions Include:  
Adafruit's MPR121 code released under BSD license https://github.com/adafruit/Adafruit_MPR121  
digitalWriteFast library to speed up pin/port manipulations: https://code.google.com/archive/p/digitalwritefast/  
There is only a benefit if pins are easily determinable at compile time (i.e., not accessed by changing variable)

There are multiple versions of this code currently:  
A single channel version that monitors only a single predetermined input in order to use digitalWriteFast  
A multiple channel version that currently monitors all MPR121 inputs and maps them all to Arduino digital outputs  
Tested on an Arduino Uno & Mega, but should work with most arduinos  
