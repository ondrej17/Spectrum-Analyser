package controller;

import model.ConfigFile;
import model.Light;
import model.Result;
import model.ResultFile;
import java.io.File;
import java.util.List;

public class Controller {

    ConfigFile configFile = new ConfigFile();
    ResultFile resultFile = new ResultFile();


    public List<Light> getLights() {
        return configFile.getLights();
    }
    public List<Result> getResults() {
        return resultFile.getResults();
    }

    public void loadFromFile(File file) {
        configFile.loadFromFile(file);
    }

    public void loadResultsFromFile(File file) {
        resultFile.loadFromFile(file);
    }

    public void saveToFile(File file) {
        configFile.saveToFile(file);
    }

}
