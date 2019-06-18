#include <stdio.h>

#include <SDL2/SDL.h>


SDL_Window *window;
SDL_Renderer *renderer;

SDL_Rect rect;
SDL_Point dir;

#define SIZE 15
#define WIDTH 800
#define HEIGHT 600
#define TITLE "Hello"

int sdl_init()
{

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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
		}
	}

	return 0;
}

void init()
{
	rect.x = 0;
	rect.y = 0;
	rect.w = SIZE;
	rect.h = SIZE;

	dir.x = 2;
	dir.y = 2;
}

void draw()
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &rect);
}

void update()
{
	rect.x += dir.x;
	rect.y += dir.y;
	if (rect.x + SIZE > WIDTH || rect.x < 0) {
		dir.x *= -1;
	}

	if (rect.y + SIZE > HEIGHT || rect.y < 0) {
		dir.y *= -1;
	}

}

void clean_up()
{
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
	init();
	while (!quit) {
		while (SDL_PollEvent(&event) == 1) {
			if (event.type == SDL_QUIT) {
				quit = 1;
			}
		}


		if (handle_input(&event)) {
			quit = 1;
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
