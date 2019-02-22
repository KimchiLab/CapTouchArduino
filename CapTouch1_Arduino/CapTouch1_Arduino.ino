/* CapTouch translation circuit:
    The Arduino monitors an MPR121 for changes in capacitance
    and essentially copies the state of MPR121 inputs to Arduino digital outputs
    This is the single channel version
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
const int interruptPin = 2; // 2 or 3 on Uno
const int ch_in = 11; // Which lines to monitor for inputs: 0 indexed. Can be 0-11 on MPR121
const int ch_out = 13; // Which lines to monitor for inputs: 0 indexed. Can be 2-13 on Arduino Uno (0-1 used for Serial Communication)
volatile int baseVal;
int diffThr = 30; // Minimum difference from baseline: Adafruit uses 12, CJ 10. 1676 on 2018/05/15: Baseline = 192, Licks = 80-100, so diffThr can equal >30 easily, Few smaller licks being missed for 3654 on same day, would be fine at 20? but diff than larger licks (bimodal?)
boolean currStatus, lastStatus;  // Status of lines: to minimize the number of digital writes: only when things change
volatile boolean flag_reset;

// Timing vars
long ms_start;
int i_loop;

// SETUP CODE: Pins and baseline
void setup() {
  Serial.begin(115200);

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
  pinMode(ch_out, OUTPUT);
  digitalWriteFast(ch_out, LOW);

  // Initialize input/interrupt
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), ResetInterrupt, RISING);

  // Get baseline data for all inputs
  delay(1000); // Capacitance values won't be as valid <1sec after bootup
  ResetBaseline();

  Serial.println("Start");
  ms_start = millis();
}

void loop() {
  if (flag_reset) {
    ResetBaseline();
  }

  // Capacitance drops with touch, hence "reverse/negative" logic
  int diffCurr = baseVal - cap.filteredData(ch_in);
  currStatus = diffCurr >= diffThr;
  if (currStatus != lastStatus) {
    // Check flags to limit the number of digitalWrite operations
    if (currStatus) {
      digitalWriteFast(ch_out, HIGH);
    } else {
      digitalWriteFast(ch_out, LOW);
    }
    lastStatus = currStatus;
  }

  //  // TTL like debugging output
  //  Serial.print(currStatus);
  //  Serial.print(" ");
  //  Serial.print(baseVal);
  //  Serial.print(" ");
  //  Serial.print(cap.baselineData(ch_in)); // Corrects too aggressively and then returns very slowly
  //  Serial.print(" ");
  //  Serial.print(cap.filteredData(ch_in));
  //  Serial.print(" ");
  //  Serial.print(diffCurr);
  //  Serial.print(" ");
  //  Serial.println();

  //  // Loop timing checks
  //  i_loop++; // Loop timing with just 1 channel: just under 600 ms for 1000 reps, so ~0.6ms/check without updates
  //  if (!(i_loop % 1000)) {
  //    Serial.print(i_loop);
  //    Serial.print(" ");
  //    Serial.println(millis() - ms_start);
  //  }
}

// ResetBaseline
// What if there is contact during this time, i.e. animal is licking?
void ResetBaseline() {
  const int numBase = 100; // Number of points to use for baseline estimates (at least 10)
  baseVal = 0; // Reset Baseline value
  for (int i_base = 0; i_base < numBase; i_base++) {
    baseVal += cap.filteredData(ch_in); // Does not work in interrupt
    delayMicroseconds(1000); // delay and millis do not work in intertupts, want to get meaningful data
  }
  // Take averages for baseline values
  baseVal = baseVal  / numBase;
  flag_reset = false;
}

void ResetInterrupt() {
  flag_reset = true;
}


