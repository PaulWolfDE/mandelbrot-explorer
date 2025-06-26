package de.paulwolf.mandelbrot.core;

public record Complex (double real, double imaginary) {

    /**
     * Squares the complex number
     *
     * @return Squared complex number
     */
    public Complex square() {
        return new Complex(real*real-imaginary*imaginary, 2*real*imaginary);
    }

    /**
     * Returns the absolute value of the complex number.
     *
     * @return Absolute value as the distance from the origin.
     */
    public double abs() {
        return Math.sqrt(real*real+imaginary*imaginary);
    }

    /**
     * Adds two complex numbers.
     *
     * @param a Complex number
     * @param b Complex number
     * @return Sum of a and b
     */
    public static Complex add(Complex a, Complex b) {
        return new Complex(a.real+b.real, a.imaginary+b.imaginary);
    }

    /**
     * Performs the mandelbrot function on a complex number.
     *
     * @param original Original complex number that is to be examined.
     * @return Result of the Mandelbrot operation
     */
    public Complex mandelbrot(Complex original) {
        return add(square(), original);
    }

    @Override
    public String toString () {
        return String.format("(%f, %f)", real, imaginary);
    }
}
