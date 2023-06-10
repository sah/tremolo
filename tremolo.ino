#include <Audio.h>
#include "effect_tremolo.h"

AudioControlSGTL5000 codec;

AudioInputI2S audioInput;
AudioEffectTremolo tremolo;
AudioMixer4 mixer;
AudioOutputI2S audioOutput;

AudioConnection inToTrem(audioInput, 0, tremolo, 0);
AudioConnection inToMixer(audioInput, 0, mixer, 0);
AudioConnection tremToMixer(tremolo, 0, mixer, 1);
AudioConnection mixerToOut(mixer, 0, audioOutput, 0);

void setup() {
    // Set the buffer size:
    AudioMemory(8);

    // Configure pins we plan to use as inputs:
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
    pinMode(A3, INPUT);

    // Enable the SGTL5000 and configure its inputs and outpus:
    codec.enable();
    codec.inputSelect(AUDIO_INPUT_LINEIN);
    codec.muteHeadphone();
    codec.micGain(0);
    codec.lineInLevel(5);
    codec.lineOutLevel(20);

    // Initial values don't matter much, since the loop will
    // immediately overwrite them.
    mixer.gain(0, 0.5);
    mixer.gain(1, 0.5);
    tremolo.begin(200);
}

void loop() {
    // 10-bit ADCs, max value is 1023.
    float speed = analogRead(A0) / 1023.0;
    float vol = analogRead(A1) / 1023.0;
    float depth = analogRead(A2)  / 1023.0;
    int shape = analogRead(A3);

    // Convert speed to 85-1000ms:
    tremolo.setSpeed(85 + (1 - speed) * 915);

    // In the mixer, depth controls balance between the wet and dry
    // channels, while vol controls the scale of both channels:
    mixer.gain(0, (1 - depth) * vol);
    mixer.gain(1, depth * vol);

    // We use waveform 0 (square wave) if shape is less than 512, and
    // 2 (sine-like parabolic wave) otherwise.
    tremolo.setWaveform(shape < 512 ? 0 : 2);
}
