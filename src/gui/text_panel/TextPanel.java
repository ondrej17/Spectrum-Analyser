package gui.text_panel;

import javax.swing.*;
import javax.swing.border.Border;
import java.awt.*;
import java.io.*;

public class TextPanel extends JPanel{
    // holds the name of spectrum that we are analysing
    public String experimentSpectrum;
    private JTextArea textArea;
    private JLabel label;

    public TextPanel() {
        Dimension dim = getPreferredSize();
        dim.width = 250;
        setPreferredSize(dim);

        Border innerBorder = BorderFactory.createTitledBorder("Status");
        Border outerBorder = BorderFactory.createEmptyBorder(5, 5, 5, 5);
        setBorder(BorderFactory.createCompoundBorder(outerBorder, innerBorder));

        textArea = new JTextArea();
        label = new JLabel("Spectrum Analyzer \u00a9 2019", SwingConstants.CENTER);

        setLayout(new BorderLayout());

        add(new JScrollPane(textArea), BorderLayout.CENTER);
        add(label, BorderLayout.SOUTH);
    }

    public void appendText(String text) {
        textArea.append(text);
    }

    public void loadLogFileKernel() {
        String workingDirectory = System.getProperty("user.dir");
        String fileName = "logFileKernel.txt";
        String absoluteFilePath = "";
        absoluteFilePath = workingDirectory + File.separator + "kernel" + File.separator + fileName;
        System.out.println("Final filepath : " + absoluteFilePath);

        BufferedReader in = null;
        try {
            in = new BufferedReader(new FileReader(absoluteFilePath));
            String str;
            while ((str = in.readLine()) != null) {
                textArea.append(str + '\n');
            }
        } catch (IOException e) {
        } finally {
            try { in.close(); } catch (Exception ex) { }
        }

    }

    public void loadLogFileAvg() {
        String workingDirectory = System.getProperty("user.dir");
        String fileName = "logFileAvg.txt";
        String absoluteFilePath = "";
        absoluteFilePath = workingDirectory + File.separator + "kernel" + File.separator + fileName;
        System.out.println("Final filepath : " + absoluteFilePath);

        BufferedReader in = null;
        try {
            in = new BufferedReader(new FileReader(absoluteFilePath));
            String str;
            while ((str = in.readLine()) != null) {
                textArea.append(str + '\n');
            }
        } catch (IOException e) {
        } finally {
            try { in.close(); } catch (Exception ex) { }
        }
    }

    public void clear() {
        textArea.setText("");
    }

    public String getExperimentSpectrum() {
        return experimentSpectrum;
    }

    public void setExperimentSpectrum(String experimentSpectrum) {
        this.experimentSpectrum = experimentSpectrum;
    }
}
