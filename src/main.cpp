#include <SDL.h>
#include <complex>
#include <iostream>
#include <cmath>

#include "mandelbrot.h"

#define WIDTH 800
#define HEIGHT 600

int argb(int a, int r, int g, int b)
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window  *win = SDL_CreateWindow("Mandelbrot Explorer",
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 800, 600);

    bool running = true;

    // scale: how many pixels represent 1 coordinate
    long double xmin = -2.5, ymin = -1.5, scale = 200;

    void *pixels;
    int pitch;
    SDL_LockTexture(tex, nullptr, &pixels, &pitch);
    Uint32 *px = (Uint32*)pixels;
    int pitch32 = pitch / 4;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {

            int it = mandelbrot_iterations(xmin + static_cast<long double>(x) / scale, ymin + static_cast<long double>(y) / scale);
            px[y * pitch32 + x] = argb(255, std::max(255-it*15, 0), std::max(255-it*15, 0), std::max(255-it*15, 0));
        }
    }

    SDL_UnlockTexture(tex);
    SDL_RenderCopy(ren, tex, nullptr, nullptr);
    SDL_RenderPresent(ren);

    while (running) {

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }
    }

    SDL_DestroyTexture(tex);   
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}