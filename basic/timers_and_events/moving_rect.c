#include <SDL2/SDL.h>

#include "event_codes.h"

struct moving_rect {
	SDL_Rect rect;
	SDL_Color color;
	SDL_Point dir;
};

static int loop_thread(void *ptr)
{
	SDL_Event event;
	SDL_Keycode keycode;
	int quit = 0;
	int err = 0;

	while (!quit) {
		while (SDL_PollEvent(&event) == 1) {
			switch (event.type) {
			case SDL_USEREVENT:
				if (event.type == SDL_USEREVENT) {
					switch (event.user.code) {
					case EVENT_CODE_DRAW:
						SDL_Log("Draw!\n");
						break;
					case EVENT_CODE_UPDATE:
						// SDL_Log("Update!\n");
						break;
					case EVENT_CODE_QUIT:
						SDL_Log("Quit!\n");
						quit = 1;
						break;
					}
				}
				break;

			}
		}
	}
}


SDL_Thread *spawn_rect()
{
	/*TODO Init struct */
	return SDL_CreateThread(loop_thread, NULL, (void *)NULL);
}

