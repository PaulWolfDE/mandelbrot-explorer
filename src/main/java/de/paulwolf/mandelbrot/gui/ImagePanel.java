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

    @Override
    public void mouseWheelMoved(MouseWheelEvent e) {

        double factor = switch(e.getWheelRotation()) {
            case 1 -> Zoom.ZOOM_FACTOR;
            case -1 -> 1/Zoom.ZOOM_FACTOR;
            default -> throw new RuntimeException("Invalid zoom factor");
        };
        double newZoom =  Math.clamp(Zoom.zoomLevel*factor, Zoom.MIN_ZOOM, Zoom.MAX_ZOOM);
        factor = newZoom/Zoom.zoomLevel; // Corrects, if zoom exceeds MIN or MAX.
        Zoom.zoomLevel = newZoom;

        System.out.println(e.getWheelRotation());
        Main.getGui().setNewImage(-2*Zoom.zoomLevel, 2*Zoom.zoomLevel, -2*Zoom.zoomLevel, 2*Zoom.zoomLevel);
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
