#ifndef effect_tremolo_h_
#define effect_tremolo_h_

#include "Arduino.h"
#include "AudioStream.h"

class AudioEffectTremolo : public AudioStream
{
public:
    AudioEffectTremolo(void):
	AudioStream(1, inputQueueArray), position(0) {}
    void begin(uint32_t milliseconds);
    virtual void update(void);
private:
    audio_block_t *inputQueueArray[1];

    // These variables store the current state of the effect.
    uint32_t samples;
    uint32_t position;
    float square_state;
    float clickless_sq_state;
    float triangle_state;
    float parabolic_state;  // pseudo-sine wave
};

#endif
