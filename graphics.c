#include <stdio.h>
#include <SDL2/SDL.h>

#include "graphics.h"

SDL_Window* window;

SDL_Renderer* renderer;

SDL_Texture* buffer;

void setup_SDL() {

	window = SDL_CreateWindow(
    "Chip 8",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    1024,
    512,
    SDL_WINDOW_RESIZABLE);

  if (window == NULL) {
    printf("Cound not create window %s\n", SDL_GetError());

  }
  renderer = SDL_CreateRenderer(window, -1, 0);

  if (renderer == NULL) {
    printf("Could not create renderer%s", SDL_GetError());
  }

  buffer = SDL_CreateTexture(
              renderer, 
              SDL_PIXELFORMAT_INDEX1LSB, 
              SDL_TEXTUREACCESS_STREAMING,
              64,
              480);

  SDL_Delay(5000);
}

void draw_SDL(char* pixels) {
  SDL_UpdateTexture(buffer, NULL, pixels , 64 * sizeof(char));

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, buffer, NULL, NULL);
  SDL_RenderPresent(renderer);
}
