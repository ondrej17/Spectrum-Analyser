package model;

import java.io.*;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

public class ResultFile {

    // config file consists of LinkedList of lights
    // "light" means: [name] + [participation]
    private List<Result> results;

    // constructor of ConfigFile creates new Linked List of lights
    public ResultFile() {
        results = new LinkedList<Result>();
    }

    public List<Result> getResults() {
        return results;
    }

    // loads all lights in config file into HashMap
    public void loadFromFile(File file) {

        // loading results
        // need to look like:
        // [name]   [participation]

        BufferedReader reader;
        try {
            reader = new BufferedReader(new FileReader(file));

            String line = reader.readLine();

            while (line != null) {
                // split a line to 4 pieces and add them into list
                String arr[] = line.split("\t");

                results.add(new Result(arr[0], Double.parseDouble(arr[1]), Double.parseDouble(arr[2]), Double.parseDouble(arr[3])));

                // System.out.println(line);
                // read next line
                line = reader.readLine();
            }
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
