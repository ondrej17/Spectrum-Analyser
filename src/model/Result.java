package model;

import java.io.Serializable;

public class Result implements Serializable {

    private String name;
    private double minimum;
    private double middle;
    private double maximum;


    public Result(String name, double minimum, double middle, double maximum) {

        this.name = name;
        this.minimum = minimum;
        this.middle = middle;
        this.maximum = maximum;
    }

    // getters

    public String getName() {
        return name;
    }

    public double getMinimum() {
        return minimum;
    }

    public double getMiddle() {
        return middle;
    }

    public double getMaximum() {
        return maximum;
    }
}