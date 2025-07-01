#include "image_wizard.h"

#include <algorithm>

#include "color.h"

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