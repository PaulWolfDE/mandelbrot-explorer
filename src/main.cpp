#include <SDL.h>
#include <iostream>
#include <format>
#include <cmath>
#include <complex>
#include <iomanip>
#include <thread>

#include "mandelbrot.h"

#define WIDTH 800
#define HEIGHT 600

int argb(int a, int r, int g, int b)
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

/**
 * HSL to RGB conversion
 * Source: https://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/
 *
 * @param h Hue 0..360
 * @param s Saturation 0..1
 * @param l Luminance 0..1
 * @return color in int format
 */
int hsl(float h, float s, float l)
{
    if (s == 0.0f) return argb(255, l*255, l*255, l*255);

    float r, g, b;
    float third = 1.0f / 3.0f;

    float t1 = l < 0.5f ? l*(1+s) : l+s-l*s;
    float t2 = 2.0f*l-t1;

    float relH = h/360.0f;

    float tr = relH + third, tg = relH, tb = relH - third;

    if (tr > 1.0f) tr -= 1.0f;
    if (tr < 0.0f) tr += 1.0f;
    if (tg > 1.0f) tg -= 1.0f;
    if (tg < 0.0f) tg += 1.0f;
    if (tb > 1.0f) tb -= 1.0f;
    if (tb < 0.0f) tb += 1.0f;

    auto color_chanel = [](float t, float t1, float t2, float third){

        if (6.0f*t < 1) return t2 + (t1-t2) * 6.0f * t;
        if (2.0f*t < 1) return t1;
        if (3.0f*t < 2) return t2 + (t1-t2) * (2.0f*third - t) * 6.0f;
        return t2;
    };

    r = color_chanel(tr, t1, t2, third);
    g = color_chanel(tg, t1, t2, third);
    b = color_chanel(tb, t1, t2, third);

    return argb(255, r*255, g*255, b*255);
}

SDL_Texture *tex;
SDL_Renderer *ren;

// scale: how many pixels represent 1 coordinate
long double xmin = -2.5L, ymin = -1.5L, scale = 200L, zoom_factor = 1.1L, zoom_factor_inv = 1.0L / zoom_factor;

int panx=0, pany=0;
bool isPanning = false;

void computeRows(int sy, int n_rows, Uint32 *px, int pitch32)
{
    for (int x = 0; x < WIDTH; x++) {

        for (int y = sy; y < sy + n_rows; y++) {

            int n = mandelbrot_iterations(xmin + static_cast<long double>(x) / scale, ymin + static_cast<long double>(y) / scale);

            const int c = 15, max = 256;
            Uint32 color = hsl(static_cast<float>((c * n / 4) % 360),
                   0.25f + static_cast<float>(n) / static_cast<float>(max) / 2.0f,
                   0.5f);

            if (n == max) color = argb(255, 0, 0, 0);

            px[y * pitch32 + x] = color;
        }
    }
}

#define PREVIEW_SCALE (1.0f / 8.0f)

void render_preview()
{
    void *pixels;
    int pitch;
    SDL_LockTexture(tex, nullptr, &pixels, &pitch);
    Uint32 *px = (Uint32*)pixels;
    int pitch32 = pitch / 4;

    int w = static_cast<int>(WIDTH*PREVIEW_SCALE);
    int h = static_cast<int>(HEIGHT*PREVIEW_SCALE);
    long double s = scale*PREVIEW_SCALE;

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {

            int n = mandelbrot_iterations(xmin + static_cast<long double>(x) / s, ymin + static_cast<long double>(y) / s);

            const int c = 15, max = 256;
            Uint32 color = hsl(static_cast<float>((c * n / 4) % 360),
                   0.25f + static_cast<float>(n) / static_cast<float>(max) / 2.0f,
                   0.5f);

            if (n == max) color = argb(255, 0, 0, 0);

            px[y * pitch32 + x] = color;
        }
    }
    SDL_UnlockTexture(tex);
    SDL_RenderCopy(ren, tex, nullptr, nullptr);
    SDL_RenderPresent(ren);
}

#define N_THREADS 8

void repaint()
{
    render_preview();
    return;
    void *pixels;
    int pitch;
    SDL_LockTexture(tex, nullptr, &pixels, &pitch);
    Uint32 *px = (Uint32*)pixels;
    int pitch32 = pitch / 4;

    std::thread threads[HEIGHT];

    for (int y = 0; y < HEIGHT; y += HEIGHT/N_THREADS)
        threads[y] = std::thread(computeRows, y, HEIGHT/N_THREADS, px, pitch32);

    for (int y = 0; y < HEIGHT; y += HEIGHT/N_THREADS)
        threads[y].join();

    SDL_UnlockTexture(tex);
    SDL_RenderCopy(ren, tex, nullptr, nullptr);
    SDL_RenderPresent(ren);

    std::cout << "done" << std::endl;
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
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 800, 600);

    bool running = true;
    repaint();

    while (running) {

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_MOUSEWHEEL) {

                int mx, my;
                SDL_GetMouseState(&mx, &my);

                long double z = e.wheel.y > 0 ? zoom_factor : zoom_factor_inv;

                long double px = xmin + static_cast<long double>(mx) / scale;
                long double py = ymin + static_cast<long double>(my) / scale;

                long double dx = (px-xmin)/z, dy = (py-ymin)/z;

                xmin = px-dx;
                ymin = py-dy;

                scale *= z;

                repaint();
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && !isPanning) {

                isPanning = true;
                panx = e.button.x;
                pany = e.button.y;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT) {

                int mx, my;
                SDL_GetMouseState(&mx, &my);
                long double px = xmin + static_cast<long double>(mx) / scale;
                long double py = ymin + static_cast<long double>(my) / scale;
                // DEBUG
                std::cout << std::setprecision(20) << px << " " << py << std::endl;
            }

            if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT && isPanning) {

                long double dx = (e.button.x - panx)/scale;
                long double dy = (e.button.y - pany)/scale;

                xmin -= dx;
                ymin -= dy;

                repaint();
                isPanning = false;
            }
        }
    }

    SDL_DestroyTexture(tex);   
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}