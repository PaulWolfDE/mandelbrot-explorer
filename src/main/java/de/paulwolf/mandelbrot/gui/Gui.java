package de.paulwolf.mandelbrot.gui;

import de.paulwolf.mandelbrot.core.ImagePanel;
import de.paulwolf.mandelbrot.core.ImageRenderer;

import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;

public class Gui extends JFrame {

    private final ImageRenderer imageRenderer = new ImageRenderer();
    private int sizeX = 800, sizeY = 600;


    public Gui() {

        BufferedImage img = imageRenderer.generateImage(sizeX, sizeY, -2, 2, -2, 2);
        ImagePanel imagePanel = new ImagePanel(img);
        this.add(imagePanel);

        this.setSize(sizeX, sizeY);
        this.setLocationRelativeTo(null);
        this.setVisible(true);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }
}
