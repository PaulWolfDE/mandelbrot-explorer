#include <SDL.h>
#include <iostream>
#include <format>
#include <cmath>
#include <iomanip>
#include <thread>
#include <mutex>

#include "mandelbrot.h"
#include "color.h"

#define WIDTH 800
#define HEIGHT 600

std::mutex scale_mutex;

/**
 * Bilinear interpolation
 * source: https://chao-ji.github.io/jekyll/update/2018/07/19/BilinearResize.html
 *
 * @param w Width of the original image
 * @param h Height of the original image
 * @param width Width of the resized image
 * @param height Height of the resized image
 * @param px Image input
 * @param input_pitch Pitch of input (width of input array)
 * @param px_out Pointer to output
 * @param output_pitch Pitch of output
 */
void bilinear_interpolation(int w, int h, int width, int height, const Uint32 *px, int input_pitch, Uint32 *px_out, int output_pitch)
{
    float x_ratio = static_cast<float>(w-1) / static_cast<float>(width-1);
    float y_ratio = static_cast<float>(h-1) / static_cast<float>(height-1);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++)
        {
            float gx = x_ratio * x;
            float gy = y_ratio * y;

            int x_l = static_cast<int>(floor(gx));
            int y_l = static_cast<int>(floor(gy));
            int x_h = static_cast<int>(ceil(gx));
            int y_h = static_cast<int>(ceil(gy));

            x_h = std::min(x_h, w-1);
            y_h = std::min(y_h, h-1);

            float x_weight = gx - x_l;
            float y_weight = gy - y_l;

            Uint32 A = px[y_l * input_pitch + x_l];
            Uint32 B = px[y_l * input_pitch + x_h];
            Uint32 C = px[y_h * input_pitch + x_l];
            Uint32 D = px[y_h * input_pitch + x_h];

            int r = ((A >> 16) & 0xFF) * (1-x_weight) * (1-y_weight) +
                    ((B >> 16) & 0xFF) * x_weight * (1-y_weight) +
                    ((C >> 16) & 0xFF) * (1-x_weight) * y_weight +
                    ((D >> 16) & 0xFF) * x_weight * y_weight;

            int g = ((A >> 8) & 0xFF) * (1-x_weight) * (1-y_weight) +
                    ((B >> 8) & 0xFF) * x_weight * (1-y_weight) +
                    ((C >> 8) & 0xFF) * (1-x_weight) * y_weight +
                    ((D >> 8) & 0xFF) * x_weight * y_weight;

            int b = (A & 0xFF) * (1-x_weight) * (1-y_weight) +
                    (B & 0xFF) * x_weight * (1-y_weight) +
                    (C & 0xFF) * (1-x_weight) * y_weight +
                    (D & 0xFF) * x_weight * y_weight;

            px_out[y * output_pitch + x] = Color(r, g, b).argb();
        }
    }
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
            Color color(static_cast<int>(static_cast<float>((c * n / 4) % 360)),
                   0.25f + static_cast<float>(n) / static_cast<float>(max) / 2.0f,
                   0.5f);

            if (n == max) color = Color(0, 0, 0);

            px[y * pitch32 + x] = color.argb();
        }
    }
}

#define PREVIEW_SCALE (1.0f / 4.0f)

void render_preview()
{
    void *pixels;
    int pitch;
    SDL_LockTexture(tex, nullptr, &pixels, &pitch);
    Uint32 *px = (Uint32*)pixels;

    Uint32 px_temp[WIDTH*HEIGHT];

    int pitch32 = pitch / 4;

    int w = static_cast<int>(WIDTH*PREVIEW_SCALE);
    int h = static_cast<int>(HEIGHT*PREVIEW_SCALE);
    long double s = scale*PREVIEW_SCALE;

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {

            int n = mandelbrot_iterations(xmin + static_cast<long double>(x) / s, ymin + static_cast<long double>(y) / s);

            const int c = 15, max = 256;
            Color color(static_cast<int>(static_cast<float>((c * n / 4) % 360)),
                   0.25f + static_cast<float>(n) / static_cast<float>(max) / 2.0f,
                   0.5f);

            if (n == max) color = Color(0, 0, 0);

            px_temp[y * pitch32 + x] = color.argb();
        }
    }

    bilinear_interpolation(w, h, WIDTH, HEIGHT, px_temp, pitch32, px, pitch32);

    SDL_UnlockTexture(tex);
    SDL_RenderCopy(ren, tex, nullptr, nullptr);
    SDL_RenderPresent(ren);
}

#define N_THREADS 8

void repaint()
{
    // render_preview();

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
                    // wait 1 s
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    std::lock_guard lock(scale_mutex);
                    if (prev_scale == scale) repaint();
                };

                std::thread t(_repaint, prev_scale);
                t.detach();
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