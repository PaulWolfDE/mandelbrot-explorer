package de.paulwolf.mandelbrot.gui;

import de.paulwolf.mandelbrot.Main;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.image.BufferedImage;

public class ImagePanel extends JPanel implements MouseWheelListener, MouseListener {

    private BufferedImage image;

    public ImagePanel() {
        this.addMouseWheelListener(this);
        this.addMouseListener(this);
        this.requestFocus();
    }

    public void setImage(BufferedImage image) {
        this.image = image;
        this.setPreferredSize(new Dimension(image.getWidth(), image.getHeight()));
        repaint();
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.drawImage(image, 0, 0, this);
    }

    private static final double ZOOM_FACTOR = 1.1;

    public static double pX = -2, pY = -2;
    public static double width = 4, height = 4;
    public static double zoomLevel = 1;

    @Override
    public void mouseWheelMoved(MouseWheelEvent e) {

        double z = e.getPreciseWheelRotation() > 0 ? ZOOM_FACTOR : 1 / ZOOM_FACTOR;

        Point m = e.getPoint();
        double mX = m.getX()/getWidth()*width+pX, mY = m.getY()/getHeight()*height+pY;
        double deltaX = (mX-pX)*z, deltaY = (mY-pY)*z;
        pX = mX-deltaX;
        pY = mY-deltaY;
        width *= z;
        height *= z;
        zoomLevel *= z;

        Main.getGui().setNewImage(pX, pX+width, pY, pY+height);

    }

    @Override
    public void mouseClicked(MouseEvent e) {
    }

    @Override
    public void mousePressed(MouseEvent e) {

    }

    @Override
    public void mouseReleased(MouseEvent e) {

    }

    @Override
    public void mouseEntered(MouseEvent e) {

    }

    @Override
    public void mouseExited(MouseEvent e) {

    }
}
