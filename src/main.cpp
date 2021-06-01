#include <Arduino.h>
#include <MIDIUSB.h>

/****** Function Declerations *****/

// Calculate midi pitch for a given note. for example:
// calcMidiPitch('C', true, 2) will return the midi pitch for C#2
int calcMidiPitch(char key, bool sharp, int octave);

// Send a midi on and off messege given midi pitch and velocity
void midiTrigger(byte pitch, byte velocity);

// Return maximum value in array data[]
int getMax(int data[]);

// Return log scaled value for 6-bit ADC
int logScale(int value);

// Handles a single drum pad connected to analog_pin
// Generates midi velocity using logScale and getMax
// Calls midiTrigger with midi_pitch
void drumPadHandle(int analog_pin, int midi_pitch);

/****** Constants *****/
const int kTresh = 10;
const int kSize = 5;
const int kKick = calcMidiPitch('C', false, 1);
const int kSnare = calcMidiPitch('E', false, 1);
const int kHat = calcMidiPitch('G', true, 3);
const int kTom = calcMidiPitch('G', false, 1);

/****** Main *****/
void setup() {
  analogReadResolution(6);
}
void loop() {
  drumPadHandle(A1, kSnare);
  drumPadHandle(A2, kKick);
  drumPadHandle(A3, kHat);
  drumPadHandle(A4, kTom);
}

/****** Function Definitions *****/

void drumPadHandle(int analog_pin, int midi_pitch) {
  int sensor_data[kSize];
  sensor_data[0] = analogRead(analog_pin);
  if (sensor_data[0] > kTresh) {
    for (int i = 1; i < kSize; i++) {
      sensor_data[i] = analogRead(analog_pin);
    }
    midiTrigger(midi_pitch, logScale(getMax(sensor_data)));
  }
  return;
}

int getMax(int data[]) {
  int max = 0;
  for (int i = 0; i < kSize; i++) {
    if (data[i] > max) {
      max = data[i];
    }
  }
  return max;
}

int logScale(int num) {
  return 127*log(num)/log(127);
}

void midiTrigger(byte key, byte velocity) {
  MidiUSB.sendMIDI({0x09, 0x90 | 0, key, velocity});
  MidiUSB.sendMIDI({0x08, 0x80 | 0, key, velocity});
  MidiUSB.flush();
}

int calcMidiPitch(char key, bool sharp, int octave) {
  int noteNum;
  switch (key)
  {
  case 'C':
    noteNum = 24;
    break;
  case 'D':
    noteNum = 26;
    break;
  case 'E':
    noteNum = 28;
    break;
  case 'F':
    noteNum = 29;
    break;
  case 'G':
    noteNum = 31;
    break;
  case 'A':
    noteNum = 33;
    break;
  case 'B':
    noteNum = 35;
    break;
  default:
    noteNum = 0;
    break;
  }
  noteNum = noteNum + 12*(octave) + sharp;
  if (noteNum > 127) {
    return 127;
  }
  return noteNum;
}