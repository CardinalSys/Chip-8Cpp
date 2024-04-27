#include <SDL.h>
#include <chrono>
#include <thread>
class Renderer {
public:
	int display[64][32] = { 0 };;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	Renderer() {


		SDL_Init(SDL_INIT_VIDEO);
		SDL_CreateWindowAndRenderer(640, 320, 0, &window, &renderer);
		SDL_RenderSetScale(renderer, 10.0f, 10.0f);

	}

	void RenderCycle() {
		SDL_Event event;

		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
		{
			SDL_DestroyWindow(window);
		}


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);


		UpdateDisplay(10);
		SDL_RenderPresent(renderer);
	}

	void UpdateDisplay(int scale) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		for (int y = 0; y < 32; y++) {
			for (int x = 0; x < 64; x++) {
				if (display[x][y] == 1) {
					SDL_RenderDrawPoint(renderer, x, y);
				}
			}
		}
	}

	void Clear() {
		for (int y = 0; y < 32; y++) {
			for (int x = 0; x < 64; x++) {
				display[x][y] = 0;
			}
		}
	}

	void SetPixel(int x,int y) {
		display[x][y] ^= 1;
	}
	
};