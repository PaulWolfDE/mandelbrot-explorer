#ifndef IMAGE_WIZARD_H
#define IMAGE_WIZARD_H
#include <SDL2/SDL_stdinc.h>

void bilinear_interpolation(int w, int h, int width, int height, const Uint32 *px, int input_pitch, Uint32 *px_out, int output_pitch);

#endif //IMAGE_WIZARD_H
