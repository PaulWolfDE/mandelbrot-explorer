#include <cmath>
#include <iomanip>

#define MAX_ITERATIONS 200

int mandelbrot_iterations(long double cx, long double cy)
{
    long double x = 0.0L;
    long double y = 0.0L;

    for (int i = 0; i < MAX_ITERATIONS; i++) {

        // z = z^2 + c
        long double x2 = x*x - y*y + cx;
        long double y2 = 2.0L * x * y + cy;

        x = x2, y = y2;

        // |z| > 2  <=> |z|^2 > 4
        if (x*x+y*y > 4.0L)
            return i;
    }
    return MAX_ITERATIONS;
}