package gui;

import javax.swing.SwingUtilities;
import java.io.File;
import java.io.IOException;
import java.io.PrintStream;

public class App {

    public static void main(String[] args) {

        // redirecting output in logFileGui.txt
        try
        {
            System.setOut(new PrintStream(new File("logFileGui.txt")));
        }
        catch(IOException e)
        {
            System.out.println("File logFileGui.txt cannot be created...");
        }

        // we need this in case multi-threading in code
        SwingUtilities.invokeLater(new Runnable() {
            // need to override run method !
            @Override
            public void run() {
                // create new object - window
                new MainFrame();
            }
        });
    }

}