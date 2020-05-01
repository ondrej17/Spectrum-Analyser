package gui.form_panel;

import java.util.EventObject;

public class FormEvent extends EventObject {
    private double minLambda;
    private double maxLambda;
    //private double angExp;
    private double deltaLambda;
    private String inputFile;

    private boolean inputFileChanged;
    private boolean doItForMeClicked;
    private boolean startClicked;
    private boolean showGraphClicked;
    private boolean showGraphOrigClicked;
    
    public FormEvent(Object source, String minLambda, String maxLambda, /*String angExp,*/
                     String deltaLambda, String inputFile, boolean inputFileChanged, boolean doItForMeClicked,
                     boolean startClicked, boolean showGraphClicked, boolean showGraphOrigClicked) {
        super(source);

        // get double from string
        this.minLambda = Double.parseDouble(minLambda);
        this.maxLambda = Double.parseDouble(maxLambda);
        this.deltaLambda = Double.parseDouble(deltaLambda);
        this.inputFileChanged = inputFileChanged;
        this.inputFile = inputFile;
        this.doItForMeClicked = doItForMeClicked;
        this.startClicked = startClicked;
        this.showGraphClicked = showGraphClicked;
        this.showGraphOrigClicked = showGraphOrigClicked;
    }

    // getters and setters
    public double getMinLambda() {
        return minLambda;
    }

    public void setMinLambda(double minLambda) {
        this.minLambda = minLambda;
    }

    public double getMaxLambda() {
        return maxLambda;
    }

    public void setMaxLambda(double maxLambda) {
        this.maxLambda = maxLambda;
    }

    public boolean isInputFileChanged() {
        return inputFileChanged;
    }

    public void setInputFileChanged(boolean inputFileChanged) {
        this.inputFileChanged = inputFileChanged;
    }

    /*public double getAngExp() {
        return angExp;
    }

    public void setAngExp(double angExp) {
        this.angExp = angExp;
    }*/

    public double getDeltaLambda() {
        return deltaLambda;
    }

    public void setDeltaLambda(double deltaLambda) {
        this.deltaLambda = deltaLambda;
    }

    public String getInputFile() {
        return inputFile;
    }

    public void setInputFile(String inputFile) {
        this.inputFile = inputFile;
    }

    public boolean isdoItForMeClicked() {
        return doItForMeClicked;
    }

    public void setdoItForMeClicked(boolean showLightsClicked) {
        this.doItForMeClicked = showLightsClicked;
    }

    public boolean isStartClicked() {
        return startClicked;
    }

    public void setStartClicked(boolean startClicked) {
        this.startClicked = startClicked;
    }

    public boolean isShowGraphClicked() {
        return showGraphClicked;
    }

    public void setShowGraphClicked(boolean showGraphClicked) {
        this.showGraphClicked = showGraphClicked;
    }

    public boolean isShowGraphOrigClicked() {
        return showGraphOrigClicked;
    }

    public void setShowGraphOrigClicked(boolean showGraphOrigClicked) {
        this.showGraphOrigClicked = showGraphOrigClicked;
    }
}