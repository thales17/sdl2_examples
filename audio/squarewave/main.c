#include <math.h>

#include <SDL2/SDL.h>


struct squarewave {
	float phase_inc;
	float phase;
	float volume;
};

void audio_callback(void *userdata, uint8_t *stream, int len)
{
	union floatbytes {
		float f;
		uint8_t bytes[4]; // Assume little endian
	};

	union floatbytes overlay;

	struct squarewave *swdata = (struct squarewave *)userdata;

	/*SDL_Log("Audio callback! len: %d\n", len);
	   SDL_Log("phase_inc: %f\n", swdata->phase_inc);
	   SDL_Log("phase: %f\n", swdata->phase);
	   SDL_Log("volume: %f\n", swdata->volume);*/

	for (int i = 0; i < len; i += 4) {
		overlay.f = swdata->volume;
		if (swdata->phase > 0.5) {
			overlay.f = -1 * swdata->volume;
		}

		stream[i + 0] = overlay.bytes[0];
		stream[i + 1] = overlay.bytes[1];
		stream[i + 2] = overlay.bytes[2];
		stream[i + 3] = overlay.bytes[3];

		swdata->phase = fmodf((swdata->phase + swdata->phase_inc), (float)1.0);
	}
}

int main()
{
	int err;

	SDL_AudioDeviceID dev;
	SDL_AudioSpec want, have;

	struct squarewave swdata;

	SDL_Log("SDL Squarewave");

	err = SDL_Init(SDL_INIT_AUDIO);
	if (err != 0) {
		SDL_Log("Init error: %s", SDL_GetError());
		return 1;
	}

	swdata.phase_inc = 140.0 / 44100.0;
	swdata.phase = 0.0;
	swdata.volume = 0.15;


	SDL_memset(&want, 0, sizeof(want));
	want.freq = 44100;
	want.format = AUDIO_F32;
	want.channels = 1;
	want.samples = 2048;
	want.userdata = &swdata;
	want.callback = audio_callback;

	dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

	SDL_Log("freq: %d, format: %d, channels: %d, samples: %d, size: %d", have.freq, have.format, have.channels, have.samples, have.size);

	if (dev == 0) {
		SDL_Log("Unable to open audio device: %s", SDL_GetError());
	}

	SDL_PauseAudioDevice(dev, 0);
	SDL_Delay(100);
	SDL_LockAudioDevice(dev);
	swdata.volume = 0;
	SDL_UnlockAudioDevice(dev);
	SDL_Delay(1000);
	SDL_LockAudioDevice(dev);
	swdata.volume = 0.25;
	swdata.phase_inc = 440.0 / 44100.0;
	swdata.phase = 0;
	SDL_UnlockAudioDevice(dev);
	SDL_Delay(250);
	SDL_PauseAudioDevice(dev, 1);

	return 0;
}
