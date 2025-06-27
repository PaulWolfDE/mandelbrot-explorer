package de.paulwolf.mandelbrot.gui;

import de.paulwolf.mandelbrot.core.ImageRenderer;

import javax.swing.*;
import java.awt.image.BufferedImage;

public class Gui extends JFrame {

    private final ImageRenderer imageRenderer = new ImageRenderer();
    private final ImagePanel imagePanel = new ImagePanel();

    private int sizeX = 800, sizeY = 600;
    private double xmin=-2, xmax=2, ymin=-2, ymax=2;

    private double zoom = 1;

    public Gui() {

        new ImagePanel();
        this.setNewImage();
        this.add(imagePanel);

        this.setSize(sizeX, sizeY);
        this.setLocationRelativeTo(null);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    private void setNewImage() {

        System.out.printf("xmin = %lf, xmax = %lf, ymin = %f, ymax = %f\n", xmin, xmax, ymin, ymax);

        BufferedImage image = imageRenderer.generateImage(this.sizeX, this.sizeY, this.xmin, this.xmax, this.ymin, this.ymax);
        imagePanel.setImage(image);
    }

    public void setNewImage(double xmin, double xmax, double ymin, double ymax) {

        this.xmin = xmin;
        this.xmax = xmax;
        this.ymin = ymin;
        this.ymax = ymax;

        this.setNewImage();
    }

}
