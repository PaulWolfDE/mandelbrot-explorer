#include <SDL.h>
#include <SDL_ttf.h>

#include <iostream>
#include <format>
#include <iomanip>
#include <thread>
#include <mutex>

#include "mandelbrot.h"
#include "color.h"
#include "image_wizard.h"

#define WIDTH 800
#define HEIGHT 600

#define MAX_ITERATIONS 256

std::mutex scale_mutex;

enum { EVT_REPAINT = SDL_USEREVENT + 1 };

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

            int n = mandelbrot_iterations(xmin + static_cast<long double>(x) / scale, ymin + static_cast<long double>(y) / scale, MAX_ITERATIONS);

            Color color(n, MAX_ITERATIONS, HSL);

            px[y * pitch32 + x] = color.argb();
        }
    }
}

#define PREVIEW_SCALE (1.0f / 4.0f)

void render_preview()
{
    int w = static_cast<int>(WIDTH*PREVIEW_SCALE);
    int h = static_cast<int>(HEIGHT*PREVIEW_SCALE);
    auto *preview = new Uint32[w*h];

    long double s = scale*PREVIEW_SCALE;

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {

            int n = mandelbrot_iterations(xmin + static_cast<long double>(x) / s, ymin + static_cast<long double>(y) / s, MAX_ITERATIONS);

            Color color(n, MAX_ITERATIONS, HSL);

            preview[y * w + x] = color.argb();
        }
    }

    auto *px = new Uint32[WIDTH*HEIGHT];
    bilinear_interpolation(w, h, WIDTH, HEIGHT, preview, w, px, WIDTH);

    SDL_UpdateTexture(tex, nullptr, px, WIDTH*4);
    SDL_RenderCopy(ren, tex, nullptr, nullptr);
    SDL_RenderPresent(ren);
}

TTF_Font *s = nullptr;
SDL_Texture *text_tex = nullptr;
SDL_Rect text_dst = {0+5, HEIGHT-14-5, 0, 0};

void render_coordinates()
{
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    long double cx = xmin + static_cast<long double>(mx) / scale;
    long double cy = ymin + static_cast<long double>(my) / scale;

    if (s == nullptr) s = TTF_OpenFont(std::format("{}cmunss.ttf", SDL_GetBasePath()).c_str(), 14);;
    if (text_tex) SDL_DestroyTexture(text_tex);

    std::string real = std::format("{:.{}f}", cx, std::max(static_cast<int>(log(scale)), 0));

    std::string imag;
    if (cy < 0)
        imag = std::format("- {:.{}f}i", abs(cy), std::max(static_cast<int>(log(scale)), 0));
    else
        imag = std::format("+ {:.{}f}i", cy, std::max(static_cast<int>(log(scale)), 0));

    std::string coordinates = std::format("({} {})", real, imag);

    SDL_Surface* surf  = TTF_RenderUTF8_Blended(s, coordinates.c_str(), {255, 255, 255});
    text_tex = SDL_CreateTextureFromSurface(ren, surf);

    text_dst.w = surf->w;
    text_dst.h = surf->h;

    SDL_FreeSurface(surf);

    SDL_RenderCopy(ren, tex, nullptr, nullptr);
    SDL_RenderCopy(ren, text_tex, nullptr, &text_dst);
    SDL_RenderPresent(ren);
}

#define N_THREADS 8

void repaint()
{
    Uint32 fb[WIDTH*HEIGHT];
    std::thread pool[N_THREADS];

    for (int i = 0; i < N_THREADS; i++)
        pool[i] = std::thread(computeRows, i*HEIGHT/N_THREADS, HEIGHT/N_THREADS, fb, WIDTH);
    for (auto &t : pool) t.join();

    SDL_UpdateTexture(tex, nullptr, fb, WIDTH*4);
    SDL_RenderCopy(ren, tex, nullptr, nullptr);
    SDL_RenderPresent(ren);

    render_coordinates();
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    TTF_Init();

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
            if (e.type == EVT_REPAINT) repaint();
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_MOUSEWHEEL)
            {
                int mx, my;
                SDL_GetMouseState(&mx, &my);

                long double z = e.wheel.y > 0 ? zoom_factor : zoom_factor_inv;

                long double px = xmin + static_cast<long double>(mx) / scale;
                long double py = ymin + static_cast<long double>(my) / scale;

                long double dx = (px-xmin)/z, dy = (py-ymin)/z;

                xmin = px-dx;
                ymin = py-dy;

                std::lock_guard lock(scale_mutex);
                scale *= z;

                render_preview();
                long double prev_scale = scale;

                auto _repaint = [](long double prev_scale) {

                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    std::lock_guard lock(scale_mutex);
                    if (prev_scale == scale) {
                        SDL_Event ev;
                        ev.type = EVT_REPAINT;
                        SDL_PushEvent(&ev);
                    }
                };

                std::thread t(_repaint, prev_scale);
                t.detach();
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && !isPanning) {

                isPanning = true;
                panx = e.button.x;
                pany = e.button.y;
            }

            if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT && isPanning) {

                long double dx = (e.button.x - panx)/scale;
                long double dy = (e.button.y - pany)/scale;

                xmin -= dx;
                ymin -= dy;

                repaint();
                isPanning = false;
            }

            if (e.type == SDL_MOUSEMOTION) {
                render_coordinates();
            }
        }
    }

    SDL_DestroyTexture(tex);   
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}