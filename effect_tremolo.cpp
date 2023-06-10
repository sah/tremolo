#include <sys/param.h>
#include "effect_tremolo.h"

// Initialize the tremolo effect with a given speed in milliseconds.
void AudioEffectTremolo::begin(uint32_t milliseconds)
{
    // These variables are stored on the AudioEffectTremolo object and
    // hold the current state of the effect.
    half_cycle_samples = milliseconds * 44.1 / 2.0;
    position = 0;
    square_state = 1;
    clickless_sq_state = 1;
    triangle_state = -1;
    parabolic_state = 0;
    waveform = 2;
}

// This function is called once for every block of AUDIO_BLOCK_SAMPLES
// samples arriving at the effect input. Our job is to receive that
// block, and transmit an output block of the same number of
// samples. Anything we need to remember from previous blocks must be
// stored on the AudioEffectTremolo object.
void AudioEffectTremolo::update(void)
{
    audio_block_t *block = receiveWritable(0);
    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
	if (position > half_cycle_samples) {
	    square_state = -square_state;
	    position = 0;
	}

	// An actual square wave will make clicking sounds at the
	// edges when the audio suddently cuts in and out. To avoid
	// that, we do a quick fade-in and fade-out instead of a
	// sudden cut.
	if (square_state > clickless_sq_state) {
	    clickless_sq_state += 0.02;
	} else if (square_state < clickless_sq_state) {
	    clickless_sq_state -= 0.02;
	}
	// Clamp the output value between -1 and 1, because float
	// point, discrete math isn't precise and we don't want any
	// little offset from the floats or sampling to add up over
	// time.
	clickless_sq_state = constrain(clickless_sq_state, -1, 1);

        // To get a triangle wave, we just add up the square samples,
	// but scale them down so that the positive cycle samples sum
	// to 2, taking our triangle wave from -1 to 1, and subtract 2
	// during the negative to take us back to -1.
	triangle_state += square_state / (half_cycle_samples / 2);
	triangle_state = constrain(triangle_state, -1, 1);

	// To get something more like a sine wave, we can add up the
	// triangle wave samples, again scaling so the wave ranges
	// from -1 to 1 (each cycle of the triangle wave has half the
	// area of a square, so we divide by 2 again). This produces a
	// parabolic wave, which is close enough to a sine wave for
	// our purposes.
	parabolic_state += triangle_state / (half_cycle_samples / (2 * 2));
	parabolic_state = constrain(parabolic_state, -1, 1);

	// Shift the final wave form into the range from 0 to 1, then
	// multiply by the audio sample to scale its volume.
	float waveform_state = clickless_sq_state;
	if (waveform == 1) {
	    waveform_state = triangle_state;
	} else if (waveform == 2) {
	    waveform_state = parabolic_state;
	}
	block->data[i] = block->data[i] * (waveform_state + 1) / 2;
	position++;
    }

    transmit(block, 0);
    release(block);
}

// Update the speed
void AudioEffectTremolo::setSpeed(uint32_t milliseconds)
{
    half_cycle_samples = milliseconds * 44.1 / 2.0;
}

// Update the waveform
void AudioEffectTremolo::setWaveform(int newWaveform)
{
    waveform = newWaveform;
}
