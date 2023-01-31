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
AudioConnection mixerToOut(mixer, 0, audioOutput, 1);

float depth = 0.7;
int speed = 100;

void setup() {
    AudioMemory(8); // Set the buffer size;

    codec.enable();
    codec.inputSelect(AUDIO_INPUT_LINEIN);
    codec.muteHeadphone();
    codec.micGain(0);
    codec.lineInLevel(5);
    codec.lineOutLevel(20);

    mixer.gain(0, 1 - depth);
    mixer.gain(1, depth);

    tremolo.begin(speed);
}

void loop() {
    // This is where we would read user interface elements to control the speed and depth.
}
