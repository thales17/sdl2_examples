#include <math.h>

#include <SDL2/SDL.h>

void gen_wave(int len, uint8_t *stream)
{
	union floatbytes {
		float f;
		uint8_t bytes[4]; // Assume little endian
	};

	union floatbytes overlay;

	float phase = 0;
	float phase_inc = 440.0 / 44100.0;
	float volume = 0.25;

	for (int i = 0; i < len; i += 4) {
		overlay.f = volume;
		if (phase > 0.5) {
			overlay.f = -1 * volume;
		}

		stream[i + 0] = overlay.bytes[0];
		stream[i + 1] = overlay.bytes[1];
		stream[i + 2] = overlay.bytes[2];
		stream[i + 3] = overlay.bytes[3];

		phase = fmodf((phase + phase_inc), (float)1.0);
	}
}

int main()
{
	int err;
	int len;
	uint8_t *stream;
	SDL_AudioDeviceID dev;
	SDL_AudioSpec want, have;

	SDL_Log("SDL Squarewave");

	err = SDL_Init(SDL_INIT_AUDIO);
	if (err != 0) {
		SDL_Log("Init error: %s", SDL_GetError());
		return 1;
	}

	SDL_memset(&want, 0, sizeof(want));
	want.freq = 44100;
	want.format = AUDIO_F32;
	want.channels = 1;
	want.samples = 4;

	dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

	SDL_Log("freq: %d, format: %d, channels: %d, samples: %d, size: %d", have.freq, have.format, have.channels, have.samples, have.size);

	if (dev == 0) {
		SDL_Log("Unable to open audio device: %s", SDL_GetError());
	}

	len = have.freq * have.samples;
	stream = (uint8_t *)malloc(len);
	gen_wave(len, stream);
	SDL_QueueAudio(dev, stream, len);
	SDL_PauseAudioDevice(dev, 0);
	SDL_Delay(4000);
	SDL_PauseAudioDevice(dev, 1);

	return 0;
}
