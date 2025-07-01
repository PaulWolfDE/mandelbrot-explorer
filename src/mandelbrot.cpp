int mandelbrot_iterations(long double cx, long double cy, int max_iterations)
{
    long double x = 0.0L;
    long double y = 0.0L;
    long double xx = 0.0L, yy = 0.0L;

    for (int i = 0; i < max_iterations; i++) {

        // z = z^2 + c
        long double x2 = xx - yy + cx;
        long double y2 = 2.0L * x * y + cy;

        x = x2, y = y2;

        xx = x*x;
        yy = y*y;

        // |z| > 2  <=> |z|^2 > 4
        if (xx+yy > 4.0L)
            return i;
    }
    return max_iterations;
}