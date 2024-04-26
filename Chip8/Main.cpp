#include <SDL.h>
#include <iostream>

void UpdateDisplay(int display[64][32], int scale, SDL_Renderer* renderer);

int main(int argc, char* argv[]) {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(640, 320, 0, &window, &renderer);
	SDL_RenderSetScale(renderer, 10.0f, 10.0f);
	
	int display[64][32] = { 0 };
	display[31][15] = 1;


	while (true) {

		SDL_Event event;

		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
		{
			SDL_DestroyWindow(window);
		}


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);


		UpdateDisplay(display, 10, renderer);
		SDL_RenderPresent(renderer);
	}

	return 0;
}

void UpdateDisplay(int display[64][32], int scale, SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 64; x++) {
			if (display[x][y] == 1) {
				SDL_RenderDrawPoint(renderer, x, y);
			}
		}
	}
}

