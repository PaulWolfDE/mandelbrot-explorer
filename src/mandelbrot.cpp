#include <cmath>

#define MAX_ITERATIONS 200

int mandelbrot_iterations(long double cx, long double cy)
{
    long double x = 0.0;
    long double y = 0.0;
    long double x2 = 0.0;
    long double y2 = 0.0;

    for (int i = 0; i < MAX_ITERATIONS; i++) {

        // cache x^2 and y^2
        x2 = x * x;
        y2 = y * y;

        // |z| > 2  <=> |z|^2 > 4
        if (x2 + y2 > 4.0)
            return i;

        // z = z^2 + c
        y = 2.0 * x * y + cy;
        x = x2 - y2 + cx;
    }
    return MAX_ITERATIONS;
}