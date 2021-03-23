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

Tips:
Wiring: Keep the wires between the touch circuit Arduino and the MPR121 as short as possible, and then again as short as possible between the MPR121 and the spout. The capacitance will work better when the spout is a little wet.

Baseline: When the arduino starts, it takes the first few seconds to determine the baseline. Baseline values will change depending on how the spout is positioned, how wet it is, etc. Because of this, if you turn on the device and then change the spout position, the baseline may no longer be a reliable one. If you can, turn on the Arduino when everything is in the right position.

Resetting the baseline: There is an ability to force a reset/new average. If the input pin 2 on the device goes HIGH, then the device will calculate a new baseline. You can do this manually, or you can connect a wire from an OpBox Arduino Mega shield: Connect pin 21 from the main Arduino Mega to Pin 2 of the touch circuit Arduino Uno. These wires can be longer. When you are using Processing to run a Serial Monitor for OpBox, you can press "R" to send the reset command.

# Configuration/Setup: Hardware
The most up to date information will typically be in the Arduino .ino files. But the following should be a fairly standard configuration, most so for the single contact version.

Connections:
Touch contact to MPR121 input:
  Connect your single ended contact wire to the MPR121 input, for single channel typically line 11  

  MPR121 to Arduino: here an Uno  
  Arduino 5V to the MPR121 Vin  
  Arduino Ground to MPR121 GND  
  Arduino SCL/A5 to MPR121 SCL  
  Arduino SDA/A4 to MPR121 SDA  

Arduino to a monitoring system: elsewhere a Mega running OpBox protocols. Neither of these is necessary for testing the system.  
  Arduino 13/LED is the TTL output for a touch to send to another system  
  Arduino 2 is the TTL input from another system for a reset to recalculate the baseline from another system  

# Configuration/Setup: Software  
Open up the CapTouch1_Arduino.ino file in the Arduino IDE (most recently tested with Arduino IDE v1.8.13).  
Make sure that the correct board is selected (e.g. Tools -> Board -> Arduino Uno)  
and the correct COM port (e.g. Tools -> Port -> COM4 (Arduino Uno)  
Uploade the code  

# Checking things work  
Open up the Arduino Serial Monitor (Tools -> Serial Monitor).  
Make sure the Baud Rate is set to 115200 baud  
You should get output similar to this:  

\CapTouch1_Arduino.ino  
Adafruit MPR121 Capacitive Touch sensor test  
Trying to connect to MPR121...  
MPR121 found!  
Start  

If so, you should be able to touch the circuit and see:  
1. A small red light on the MPR121 indicating contact  
2. A small orange light on the Arduino indicating contact  

# Setup/Wiring Pictures
## Setup with MPR121 Breakout Board and an Arduino Uno
![Setup with MPR121 Breakout Board and an Arduino Uno](/CapTouch1_Arduino/Setup_Breakout.jpg)

## Setup with MPR121 Shield and an Arduino Uno
![Setup with MPR121 Shield and an Arduino Uno](/CapTouch1_Arduino/Setup_Shield.jpg)
