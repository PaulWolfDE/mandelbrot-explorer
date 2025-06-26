package de.paulwolf.mandelbrot.core;

import java.awt.image.BufferedImage;

public class ImageRenderer {

    public BufferedImage generateImage(int width, int height, double xmin, double xmax, double ymin, double ymax) {

        BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);

        for (int w = 0; w < width; w++) {

            double x = xmin+(xmax-xmin)*w/width;

            for (int h = 0; h < height; h++) {

                double y = ymin+(ymax-ymin)*h/height;

                Complex c = new Complex(x, y);
                int nInterations = Mandelbrot.mandelbrotIterations(c);

                image.setRGB(w, h, Mandelbrot.iterationsToColor(nInterations));
            }
        }
        return image;
    }

}
