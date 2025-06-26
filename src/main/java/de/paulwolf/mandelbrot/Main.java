package de.paulwolf.mandelbrot;

import de.paulwolf.mandelbrot.core.Complex;
import de.paulwolf.mandelbrot.core.Mandelbrot;

import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;

public class Main {

    private static void showImage(BufferedImage image) {

        JPanel panel = new JPanel() {

            @Override
            protected void paintComponent(Graphics g) {

                super.paintComponent(g);
                g.drawImage(image, 0, 0, this);
            }
        };

        JFrame frame = new JFrame();
        frame.add(panel);
        frame.setSize(image.getWidth(), image.getHeight());
        frame.setVisible(true);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    public static void main(String[] args) {

        int width = 800, height = 800;
        double xmin = -2, xmax = 2, ymin = -2, ymax = 2;
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

        showImage(image);
    }
}