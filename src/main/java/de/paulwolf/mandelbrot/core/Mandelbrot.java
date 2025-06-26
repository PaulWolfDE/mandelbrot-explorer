package de.paulwolf.mandelbrot.core;

import java.awt.*;

public class Mandelbrot {

    public static int maxIterations = 500;
    public static int contrast = 20;

    public static int mandelbrotIterations(Complex c) {

        Complex v = new Complex(c.real(),c.imaginary());

        for (int i = 0; i < maxIterations; i++) {

            v = v.mandelbrot(c);
            if (v.abs() > 2)
                return i;
        }
        return -1;
    }

    public static int iterationsToColor(int iterations) {

        if (iterations == -1)
            return Color.BLACK.getRGB();
        return new Color(
                Math.max(255 - iterations*contrast, 0),
                Math.max(255 - iterations*contrast, 0),
                Math.max(255 - iterations*contrast, 0),
                255).getRGB();

    }


}
