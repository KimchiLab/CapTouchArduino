/* CapTouch translation circuit:
    The Arduino monitors an MPR121 for changes in capacitance
    and essentially copies the state of MPR121 inputs to Arduino digital outputs
    This is the multiple sensor version
    Code below in part by Eyal Kimchi, 2018

    Various versions Include:
    Adafruit's MPR121 code released under BSD license https://github.com/adafruit/Adafruit_MPR121
    digitalWriteFast library to speed up pin/port manipulations: https://code.google.com/archive/p/digitalwritefast/
    There is only a benefit if pins are easily determinable at compile time (i.e., not accessed by changing variable)

    There are multiple versions of this code currently:
    A single channel version that monitors only a single predetermined input in order to use digitalWriteFast
    A multiple channel version that currently monitors all MPR121 inputs and maps them all to Arduino digital outputs
    Tested on an Arduino Uno & Mega, but should work with most arduinos
*/

#include "Adafruit_MPR121.h"
#include "digitalWriteFast.h"

Adafruit_MPR121 cap = Adafruit_MPR121();

// Parameters
const int idxCh[] = {11}; // Which lines to monitor for inputs: 0 indexed. Can be 0-11 on MPR121
const int numCh = sizeof(idxCh) / sizeof(int);
const int numBase = 100; // Number of points to use for baseline estimates (at least 10)
int baseMatrix[numCh][numBase]; // To record each baseline measurement. Not necessary, can do running sum? Constant running sum is too computationally intense given high sampling rate, could all be within 1 lick
int baseArray[numCh]; // Baseline measurements for comparison later
int diffThr = 30; // Minimum difference from baseline: Adafruit uses 12. Tested on 2018/05/15: Baseline = 192, Licks = 80-100, so diffThr can equal >30 easily, Few smaller licks being missed for second subject on same day, would be fine at 20, but diff than larger licks (bimodal)
// Timing variables
unsigned long i_loop;
int ms_start;

// Status of lines: to minimize the number of digital writes only when things change
boolean currStatus[numCh];
boolean lastStatus[numCh];

// Pin translation from input to output.
// Reserve pins 0-1 for USB Serial communication
// outputs = inputs + 2, so 0-11 -> 2-13
int num_shift = 2; // Shift channel in -> channel out: from 0 -> 2 to avoid serial pins


// SETUP CODE: Pins and baseline
void setup() {
  Serial.begin(115200);
  Serial.println( __FILE__ );

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }

  Serial.println("Adafruit MPR121 Capacitive Touch sensor test");

  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  // Initialize output pins
  for (int i_ch = 0; i_ch <= numCh; i_ch++) {
    int ch_out = idxCh[i_ch] + num_shift;
    pinMode(ch_out, OUTPUT);
    digitalWriteFast(ch_out, LOW);
  }

  // Get baseline data for all inputs
  delay(1000); // Capacitance values won't be as valid <1sec after bootup
  for (int i_base = 0; i_base < numBase; i_base++) {
    for (int i_ch = 0; i_ch < numCh; i_ch++) {
      baseMatrix[i_ch][i_base] = cap.filteredData(idxCh[i_ch]);
      baseArray[i_ch] += baseMatrix[i_ch][i_base];
      delay(10); // In case repeating just a few lines/channels
    }
  }
  // Take averages for baseline values
  for (int i_ch = 0; i_ch < numCh; i_ch++) {
    baseArray[i_ch] = baseArray[i_ch] / numBase;
    // Serial.print(cap.baselineData(idxCh[i_ch])); // Library moving baseline corrects too aggressively and then returns higher too slowly
  }
  // Set start time for tracking purposes
  ms_start = millis();
  Serial.println("Start");
}

void loop() {
  //  Serial.println(i_loop);

  for (int i_ch = 0; i_ch < numCh; i_ch++) {
    // Capacitance drops with touch, hence "reverse/negative" logic
    int diffCurr = baseArray[i_ch] - cap.filteredData(idxCh[i_ch]);
    currStatus[i_ch] = diffCurr >= diffThr;
    //currStatus[i_ch] = (baseArray[i_ch] - cap.filteredData(idxCh[i_ch])) >= diffThr; // Not accurate (int -> bool?)
    int ch_out = idxCh[i_ch] + num_shift;
    if (currStatus[i_ch] != lastStatus[i_ch]) {
      // Check flags to limit the number of digitalWrite operations
      if (currStatus[i_ch]) {
        digitalWrite(ch_out, HIGH);
      } else {
        digitalWrite(ch_out, LOW);
      }
      lastStatus[i_ch] = currStatus[i_ch];
    }

    //    // TTL like debugging output
    //    Serial.print(i_ch);
    //    Serial.print(" ");
    //    Serial.print(idxCh[i_ch]);
    //    Serial.print(" ");
    //    Serial.print(currStatus[i_ch]);
    //    Serial.print(" ");
    //    Serial.print(idxCh[i_ch] + num_shift);
    //    Serial.print(" ");
    //    Serial.print(baseArray[i_ch]);
    //    Serial.print(" ");
    //    Serial.print(cap.baselineData(idxCh[i_ch])); // Corrects too aggressively and then returns very slowly
    //    Serial.print(" ");
    //    Serial.print(cap.filteredData(idxCh[i_ch]));
    //    Serial.print(" ");

  }
}


