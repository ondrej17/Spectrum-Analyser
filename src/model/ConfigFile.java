package model;

import java.io.*;
import java.util.*;

public class ConfigFile {

    // config file consists of LinkedList of lights
    // "light"  means: [name] + [participation]
    private List<Light> lights;

    // constructor of ConfigFile creates new Linked List of lights
    public ConfigFile() {
        lights = new LinkedList<Light>();
    }

    public List<Light> getLights() {
        return lights;
    }

    // loads all lights in config file into HashMap
    public void loadFromFile(File file) {

        // loading lights
        // need to look like:
        // [name]   [participation]

        BufferedReader reader;
        try {
            reader = new BufferedReader(new FileReader(file));

            String line = reader.readLine();

            while (line != null) {
                // split a line to 2 pieces and add them into list
                String arr[] = line.split("\t");

                if (arr[1].equals("1")) {
                    lights.add(new Light(arr[0], true));
                } else {
                    lights.add(new Light(arr[0], false));
                }


                // System.out.println(line);
                // read next line
                line = reader.readLine();
            }
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void saveToFile(File file) {

        // loading results
        // need to look like:
        // [name]   [participation]

        PrintWriter writer;
        try {
            writer = new PrintWriter(file, "UTF-8");

            for (Light light: lights) {
                if (light.getParticipation()) {
                    String line = String.format("%s\t1\n", light.getName());
                    writer.print(line);
                } else {
                    String line = String.format("%s\t0\n", light.getName());
                    writer.print(line);
                }
            }

            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
