package de.paulwolf.mandelbrot;

import de.paulwolf.mandelbrot.gui.Gui;


public class Main {

    private static final Gui gui = new Gui();

    public static void main(String[] args) {

        gui.setVisible(true);
    }

    public static Gui getGui() {
        return gui;
    }
}