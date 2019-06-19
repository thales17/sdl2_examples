#include <stdio.h>

#include <SDL2/SDL.h>


#define WIDTH 800
#define HEIGHT 600
#define TITLE "Simple Synth"


struct squarewave {
	float phase_inc;
	float phase;
	float volume;
};

SDL_Window *window;
SDL_Renderer *renderer;

SDL_AudioDeviceID audio_dev;
SDL_AudioSpec have_spec;

struct squarewave synthdata[4];

int b1_state = 0;
int b2_state = 0;
int b3_state = 0;
int b4_state = 0;

int update_needed = 0;

void audio_callback(void *userdata, uint8_t *stream, int len)
{
	int i;
	int j;
	union floatbytes {
		float f;
		uint8_t bytes[4];
	};

	union floatbytes overlay;
	struct squarewave *swdata = (struct squarewave *)userdata;

	for (i = 0; i < len; i += 4) {
		overlay.f = 0;
		for (j = 0; j < 4; j++) {
			overlay.f += (swdata[j].phase < 0.5) ? swdata[j].volume : -1 * swdata[j].volume;
		}

		for (j = 0; j < 4; j++) {
			stream[i + j] = overlay.bytes[j];
		}

		for (j = 0; j < 4; j++) {
			swdata[j].phase = fmodf((swdata[j].phase + swdata[j].phase_inc), (float)1.0);
		}
	}
}

int sdl_init()
{

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		SDL_Log("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_Log("Could not create renderer: %s\n", SDL_GetError());
		return 1;
	}

	return 0;
}

int handle_input(SDL_Event *e)
{
	if (e->type == SDL_KEYDOWN) {
		SDL_Keycode keycode = e->key.keysym.sym;
		if (keycode == SDLK_ESCAPE) {
			return 1;
		} else if (keycode == SDLK_a) {
			b1_state = 1;
			update_needed = 1;
		} else if (keycode == SDLK_s) {
			b2_state = 1;
			update_needed = 1;
		} else if (keycode == SDLK_d) {
			b3_state = 1;
			update_needed = 1;
		} else if (keycode == SDLK_f) {
			b4_state = 1;
			update_needed = 1;
		}
	} else if (e->type == SDL_KEYUP) {
		SDL_Keycode keycode = e->key.keysym.sym;
		if (keycode == SDLK_a) {
			b1_state = 0;
			update_needed = 1;
		} else if (keycode == SDLK_s) {
			b2_state = 0;
			update_needed = 1;
		} else if (keycode == SDLK_d) {
			b3_state = 0;
			update_needed = 1;
		} else if (keycode == SDLK_f) {
			b4_state = 0;
			update_needed = 1;
		}
	}

	return 0;
}

int init_audio()
{
	SDL_AudioSpec want;

	synthdata[0].phase_inc = 25.0 / 44100.0;
	synthdata[0].phase = 0.0;
	synthdata[0].volume = 0.0;

	synthdata[1].phase_inc = 75.0 / 44100.0;
	synthdata[1].phase = 0.0;
	synthdata[1].volume = 0.0;

	synthdata[2].phase_inc = 150.0 / 44100.0;
	synthdata[2].phase = 0.0;
	synthdata[2].volume = 0.0;

	synthdata[3].phase_inc = 440.0 / 44100.0;
	synthdata[3].phase = 0.0;
	synthdata[3].volume = 0.0;


	SDL_memset(&want, 0, sizeof(want));
	want.freq = 44100;
	want.format = AUDIO_F32;
	want.channels = 1;
	want.samples = 2048;
	want.userdata = &synthdata;
	want.callback = audio_callback;

	audio_dev = SDL_OpenAudioDevice(NULL, 0, &want, &have_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

	SDL_Log("freq: %d, format: %d, channels: %d, samples: %d, size: %d", have_spec.freq, have_spec.format, have_spec.channels, have_spec.samples, have_spec.size);
	if (audio_dev == 0) {
		SDL_Log("Unable to open audio device: %s", SDL_GetError());
		return 1;
	}

	SDL_PauseAudioDevice(audio_dev, 0);

	return 0;
}

void init()
{

}

void draw()
{
	SDL_Rect r1 = { 285, 275, 50, 50 };
	SDL_Rect r2 = { 345, 275, 50, 50 };
	SDL_Rect r3 = { 405, 275, 50, 50 };
	SDL_Rect r4 = { 465, 275, 50, 50 };

	if (b1_state) {
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	} else {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	}
	SDL_RenderFillRect(renderer, &r1);
	if (b2_state) {
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	} else {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	}

	SDL_RenderFillRect(renderer, &r2);
	if (b3_state) {
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	} else {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	}

	SDL_RenderFillRect(renderer, &r3);
	if (b4_state) {
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	} else {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	}

	SDL_RenderFillRect(renderer, &r4);
}

void update()
{
	if (!update_needed) {
		return;
	}

	SDL_LockAudioDevice(audio_dev);
	synthdata[0].volume = (b1_state) ? 0.15 : 0.0;
	synthdata[1].volume = (b2_state) ? 0.15 : 0.0;
	synthdata[2].volume = (b3_state) ? 0.15 : 0.0;
	synthdata[3].volume = (b4_state) ? 0.15 : 0.0;
	SDL_UnlockAudioDevice(audio_dev);

	update_needed = 0;
}

void clean_up()
{
	SDL_PauseAudioDevice(audio_dev, 1);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main()
{
	int err;
	int quit = 0;
	SDL_Event event;

	err = sdl_init();
	if (err != 0) {
		SDL_Log("Unable to initialize");
		return 1;
	}

	err = init_audio();
	if (err != 0) {
		return 1;
	}

	init();
	while (!quit) {
		while (SDL_PollEvent(&event) == 1) {
			if (event.type == SDL_QUIT) {
				quit = 1;
			}
			if (handle_input(&event)) {
				quit = 1;
			}

		}


		update();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		draw();
		SDL_RenderPresent(renderer);
	}

	clean_up();

	return 0;
}
