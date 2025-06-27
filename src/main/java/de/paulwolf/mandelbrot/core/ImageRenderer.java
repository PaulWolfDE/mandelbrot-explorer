package de.paulwolf.mandelbrot.core;

import java.awt.image.BufferedImage;
import java.util.stream.IntStream;

public class ImageRenderer {

    public BufferedImage generateImage(int width, int height, double xmin, double xmax, double ymin, double ymax) {

        BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);

        IntStream.range(0, width).parallel().forEach(w -> {
            double x = xmin + (xmax - xmin) * w / width;

            for (int h = 0; h < height; h++) {
                double y = ymin + (ymax - ymin) * h / height;
                Complex c = new Complex(x, y);
                int nIterations = Mandelbrot.mandelbrotIterations(c);
                image.setRGB(w, h, Mandelbrot.iterationsToColor(nIterations));
            }
        });

        return image;
    }

}
