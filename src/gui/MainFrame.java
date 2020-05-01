package gui;

import controller.Controller;
import gui.form_panel.FormEvent;
import gui.form_panel.FormListener;
import gui.form_panel.FormPanel;
import gui.light_table.LightTablePanel;
import gui.results_table.ResultTablePanel;
import gui.text_panel.TextPanel;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.io.*;
import java.net.URL;

import static javax.swing.JOptionPane.*;

public class MainFrame extends JFrame {

    // components of our window
    private LightTablePanel tablePanelInput;
    private ResultTablePanel tablePanelOutput;
    private FormPanel formPanel;
    private TextPanel textPanel;

    private JFileChooser configFileChooser;
    private JFileChooser graphFileChooser;
    private Controller controller;

    // constructor of object gui.MainFrame
    public MainFrame() {

        // set a name of the window
        super("Spectrum Analyzer");

        Cursor normalCursor = new Cursor(Cursor.DEFAULT_CURSOR);
        Cursor hourglassCursor = new Cursor(Cursor.WAIT_CURSOR);
        setCursor(normalCursor);

        // set layout (we chose the BorderLayout)
        setLayout(new GridBagLayout());

        // create an important object - gc
        GridBagConstraints gc = new GridBagConstraints();

        // something that needs to be done
        gc.fill = GridBagConstraints.BOTH;

        // set components
        tablePanelInput = new LightTablePanel();
        tablePanelOutput = new ResultTablePanel();
        formPanel = new FormPanel();
        textPanel = new TextPanel();
        setJMenuBar(createMenuBar());

        configFileChooser = new JFileChooser();
        graphFileChooser = new JFileChooser();
        controller = new Controller();

        // load data to table
        tablePanelInput.setData(controller.getLights());
        tablePanelOutput.setResults(controller.getResults());

        // load default light database to inputTable
            tablePanelInput.clearData();

            // absolute path to inputFile
            String workingDirectory = System.getProperty("user.dir"); // ........\SMOG
            String filename = "list_of_lights_generic.txt";
            String absoluteFilePath = "";
            absoluteFilePath = workingDirectory + File.separator + "kernel" + File.separator +
                    "input_data" + File.separator + filename;
            System.out.println("Final filepath : " + absoluteFilePath);

            // load inputFile to upper table
            controller.loadFromFile(new File(absoluteFilePath));
            tablePanelInput.refreshData();

        // set up fileChooser and filters
        configFileChooser.addChoosableFileFilter(new LightFileFilter());
        graphFileChooser.addChoosableFileFilter(new GraphFileFilter());

        // add components to GridLayouts
            // FIRST ROW
            gc.gridy = 0;

                // add formPanel at [0, 0]
                gc.weightx = 0.2;
                gc.weighty = 1;
                gc.gridx = 0;
                gc.anchor = GridBagConstraints.CENTER;
                gc.insets = new Insets(0, 0, 0, 0);
                add(formPanel, gc);

                // add tablePanelInput at [1, 0]
                gc.weightx = 3;
                gc.weighty = 1;
                gc.gridx = 1;
                gc.anchor = GridBagConstraints.CENTER;
                gc.insets = new Insets(0, 0, 0, 0 );
                add(tablePanelInput, gc);

            // SECOND ROW
            gc.gridy = 1;

                // add textPanel at [0, 1]
                gc.weightx = 0.2;
                gc.weighty = 1;
                gc.gridx = 0;
                gc.anchor = GridBagConstraints.CENTER;
                gc.insets = new Insets(0, 0, 0, 0);
                add(textPanel, gc);

                // add tablePanelOutput at [1, 1]
                gc.weightx = 3;
                gc.weighty = 2;
                gc.gridx = 1;
                gc.anchor = GridBagConstraints.CENTER;
                gc.insets = new Insets(0, 0, 0, 0 );
                add(tablePanelOutput, gc);


        // if something happen in formPanel, this will be done
        formPanel.setFormListener(new FormListener() {
            public void formEventOccurred(FormEvent e) {
                if (e.isInputFileChanged()) {
                    // this will happen when showLightBtn was clicked
                    // clean the table
                    tablePanelInput.clearData();

                    // absolute path to inputFile
                    String workingDirectory = System.getProperty("user.dir"); // ........\SMOG
                    String filename = e.getInputFile();
                    String absoluteFilePath = "";
                    absoluteFilePath = workingDirectory + File.separator + "kernel" + File.separator +
                            "input_data" + File.separator + filename;
                    System.out.println("Final filepath : " + absoluteFilePath);

                    // load inputFile to upper table
                    controller.loadFromFile(new File(absoluteFilePath));
                    tablePanelInput.refreshData();
                }

                if (e.isdoItForMeClicked()) {
                    // this will happen when doItForMe was clicked
                    // absolute path to inputFile
                    String workingDirectory = System.getProperty("user.dir");
                    String inputFileName = e.getInputFile();
                    String absoluteFilePath = "";
                    absoluteFilePath = workingDirectory + File.separator + "kernel" + File.separator +
                            "input_data" + File.separator + inputFileName;
                    System.out.println("Final filepath : " + absoluteFilePath);

                    // save to inputFile
                    controller.saveToFile(new File(absoluteFilePath));

                    String absoluteProgramPath = "";
                    String programName = "";

                    // find absolute path to kernel
                    programName = "";
                    absoluteProgramPath = "";
                    if(System.getProperty("os.name").toUpperCase().indexOf("WIN") >= 0) {//windows configuration
                        programName = "run_hard_work.cmd";
                        absoluteProgramPath = workingDirectory + File.separator + "kernel" + File.separator + programName;
                        System.out.println("Final filepath : " + absoluteProgramPath);

                        // create run.cmd file
                        try (PrintWriter writer = new PrintWriter(absoluteProgramPath, "UTF-8")){
                            // write a line in run_hard_work.cmd file
                            writer.print("hard_work " + e.getMinLambda() + " " + e.getMaxLambda() + " "/* + e.getAngExp()
                                    + " "*/ + e.getDeltaLambda() + " " + e.getInputFile()
                                    + " " + textPanel.getExperimentSpectrum());

                            System.out.println("hard_work " + e.getMinLambda() + " " + e.getMaxLambda() + " "/* + e.getAngExp()
                                    + " "*/ + e.getDeltaLambda() + " " + e.getInputFile()
                                    + " " + textPanel.getExperimentSpectrum());
                        } catch (FileNotFoundException | UnsupportedEncodingException ev) {
                            ev.printStackTrace();
                        }
                    }
                    if(System.getProperty("os.name").toUpperCase().indexOf("NUX") >= 0) { //linux configuration
                        programName = "run_hard_work.sh";
                        absoluteProgramPath = workingDirectory + File.separator + "kernel" + File.separator + programName;
                        System.out.println("Final filepath : " + absoluteProgramPath);

                        // create run.sh file
                        try (PrintWriter writer = new PrintWriter(absoluteProgramPath, "UTF-8")){
                            // write a line in run_hard_work.sh file
                            writer.print("./hard_work " + e.getMinLambda() + " " + e.getMaxLambda() + " "/* + e.getAngExp()
                                    + " "*/ + e.getDeltaLambda() + " " + e.getInputFile()
                                    + " " + textPanel.getExperimentSpectrum());

                            System.out.println("./hard_work " + e.getMinLambda() + " " + e.getMaxLambda() + " "/* + e.getAngExp()
                                    + " "*/ + e.getDeltaLambda() + " " + e.getInputFile()
                                    + " " + textPanel.getExperimentSpectrum());
                        } catch (FileNotFoundException | UnsupportedEncodingException ev) {
                            ev.printStackTrace();
                        }
                    }
                    if(System.getProperty("os.name").toUpperCase().indexOf("MAC") >= 0) { //mac configuration
                        // do something here
                    }

                    // run program HARD_WORK
                    ProcessBuilder pb = new ProcessBuilder(absoluteProgramPath);
                    pb.directory(new File(workingDirectory + "/kernel"));
                    if (textPanel.getExperimentSpectrum() != null) {
                        try {
                            textPanel.clear();
                            setCursor(hourglassCursor);
                            Process process = pb.start();
                            System.out.println("Program HARD_WORK started");


                            InputStream is = process.getInputStream();

                            BufferedReader br = new BufferedReader (new InputStreamReader (is));

                            String aux = br.readLine();
                            while (aux!=null) {
                                System.out.println (aux);
                                //textPanel.appendText(aux + '\n');

                                aux = br.readLine();
                            }

                            //textPanel.appendText("\n\n");
                            //textPanel.appendText("Optimum Found!\n\n");

                            // wait for smog.exe to terminate
                            int result = process.waitFor();
                            if(result == 0) {
                                setCursor(normalCursor);
                                // clear textPanel
                                //textPanel.clear();
                                // write which spectrum in being analysed
                                textPanel.appendText("SPECTRUM: \n" + textPanel.getExperimentSpectrum() + "\n" +
                                        "*****************************************\n");
                                // load log file from smog to textPanel
                                textPanel.loadLogFileKernel();
                                // load log file of spectrumavg to textpanel
                                textPanel.loadLogFileAvg();

                                // this will happen when clearTableBtn was clicked
                                // clean table
                                tablePanelOutput.clearResults();

                                // get absolute path file
                                String resultFileName = "percentage.txt";
                                absoluteFilePath = workingDirectory + File.separator + "kernel" + File.separator +
                                        "results" + File.separator + resultFileName;
                                System.out.println("Final filepath : " + absoluteFilePath);

                                // load results
                                controller.loadResultsFromFile(new File(absoluteFilePath));
                                tablePanelOutput.refreshResults();

                                // load new configuration
                                // load default light database to inputTable
                                tablePanelInput.clearData();

                                // absolute path to inputFile
                                String filename = e.getInputFile();
                                absoluteFilePath = workingDirectory + File.separator + "kernel" + File.separator +
                                        "input_data" + File.separator + filename;
                                System.out.println("Final filepath : " + absoluteFilePath);

                                // load inputFile to upper table
                                controller.loadFromFile(new File(absoluteFilePath));
                                tablePanelInput.refreshData();
                            }
                            else
                            {
                                JOptionPane.showMessageDialog(new JFrame(),
                                        "An error occured during task solving. Look into 'logFileKernel.txt' and" +
                                                " logFileAvg.txt for more details.",
                                        "Result", JOptionPane.ERROR_MESSAGE);
                                System.out.println("An error occured during task solving");
                                setCursor(normalCursor);
                            }
                        } catch (IOException | InterruptedException e1) {
                            e1.printStackTrace();
                        }
                    }
                    else {
                        JOptionPane.showMessageDialog(new JFrame(),
                                "Choose spectrum first: FILE -> Load Spectrum",
                                "Warning", JOptionPane.ERROR_MESSAGE);
                        System.out.println("An error occured during task solving");
                    }

                }
                else if (e.isStartClicked()) {
                    // this will happen when startBtn was clicked
                    // absolute path to inputFile
                    String workingDirectory = System.getProperty("user.dir");
                    String inputFileName = e.getInputFile();
                    String absoluteFilePath = "";
                    absoluteFilePath = workingDirectory + File.separator + "kernel" + File.separator +
                            "input_data" + File.separator + inputFileName;
                    System.out.println("Final filepath : " + absoluteFilePath);

                    // save to inputFile
                    controller.saveToFile(new File(absoluteFilePath));

                    // find absolute path to kernel
                    String programName = "";
                    String absoluteProgramPath = "";
                    if(System.getProperty("os.name").toUpperCase().indexOf("WIN") >= 0) {//windows configuration
                        programName = "run.cmd";
                        absoluteProgramPath = workingDirectory + File.separator + "kernel" + File.separator + programName;
                        System.out.println("Final filepath : " + absoluteProgramPath);

                        // create run.cmd file
                        try (PrintWriter writer = new PrintWriter(absoluteProgramPath, "UTF-8")){
                            // write a line in run.cmd file
                            writer.print("smog " + e.getMinLambda() + " " + e.getMaxLambda() + " "/* + e.getAngExp()
                                    + " "*/ + e.getDeltaLambda() + " " + e.getInputFile()
                                    + " " + textPanel.getExperimentSpectrum());

                            System.out.println("smog " + e.getMinLambda() + " " + e.getMaxLambda() + " "/* + e.getAngExp()
                                    + " "*/ + e.getDeltaLambda() + " " + e.getInputFile()
                                    + " " + textPanel.getExperimentSpectrum());
                        } catch (FileNotFoundException | UnsupportedEncodingException ev) {
                            ev.printStackTrace();
                        }
                    }
                    if(System.getProperty("os.name").toUpperCase().indexOf("NUX") >= 0) { //linux configuration
                        programName = "run.sh";
                        absoluteProgramPath = workingDirectory + File.separator + "kernel" + File.separator + programName;
                        System.out.println("Final filepath : " + absoluteProgramPath);

                        // create run.sh file
                        try (PrintWriter writer = new PrintWriter(absoluteProgramPath, "UTF-8")){
                            // write a line in run.cmd file
                            writer.print("./smog " + e.getMinLambda() + " " + e.getMaxLambda() + " "/* + e.getAngExp()
                                    + " "*/ + e.getDeltaLambda() + " " + e.getInputFile()
                                    + " " + textPanel.getExperimentSpectrum());

                            System.out.println("./smog " + e.getMinLambda() + " " + e.getMaxLambda() + " "/* + e.getAngExp()
                                    + " "*/ + e.getDeltaLambda() + " " + e.getInputFile()
                                    + " " + textPanel.getExperimentSpectrum());
                        } catch (FileNotFoundException | UnsupportedEncodingException ev) {
                            ev.printStackTrace();
                        }
                    }
                    if(System.getProperty("os.name").toUpperCase().indexOf("MAC") >= 0) { //mac configuration
                        // do something here
                    }


                    // run program
                    ProcessBuilder pb = new ProcessBuilder(absoluteProgramPath);
                    pb.directory(new File(workingDirectory + "/kernel"));
                    if (textPanel.getExperimentSpectrum() != null) {
                        try{
                            setCursor(hourglassCursor);
                            Process process = pb.start();
                            System.out.println("Program SMOG started");

                            // wait for smog.exe to terminate
                            int result = process.waitFor();
                            if(result == 0) {
                                setCursor(normalCursor);
                                // clear textPanel
                                textPanel.clear();
                                // write which spectrum in being analysed
                                textPanel.appendText("SPECTRUM: \n" + textPanel.getExperimentSpectrum() + "\n" +
                                        "*****************************************\n");
                                // load log file from smog to textPanel
                                textPanel.loadLogFileKernel();
                                // load log file of spectrumavg to textpanel
                                textPanel.loadLogFileAvg();

                                // this will happen when clearTableBtn was clicked
                                // clean table
                                tablePanelOutput.clearResults();

                                // get absolute path file
                                String resultFileName = "percentage.txt";
                                absoluteFilePath = workingDirectory + File.separator + "kernel" + File.separator +
                                        "results" + File.separator + resultFileName;
                                System.out.println("Final filepath : " + absoluteFilePath);

                                // load results
                                controller.loadResultsFromFile(new File(absoluteFilePath));
                                tablePanelOutput.refreshResults();
                            }
                            else
                            {
                                JOptionPane.showMessageDialog(new JFrame(),
                                        "An error occured during task solving. Look into 'logFileKernel.txt' and" +
                                                " logFileAvg.txt for more details.",
                                        "Result", JOptionPane.ERROR_MESSAGE);
                                System.out.println("An error occured during task solving");
                            }
                        } catch (IOException | InterruptedException e1) {
                            e1.printStackTrace();
                        }


                    }
                    else {
                        JOptionPane.showMessageDialog(new JFrame(),
                                "Choose spectrum first: FILE -> Load Spectrum",
                                "Warning", JOptionPane.ERROR_MESSAGE);
                        System.out.println("An error occured during task solving");
                    }


                }
                else if (e.isShowGraphClicked()) {
                    // this will happen when showGraphBtn was clicked
                    // shows graph
                    String workingDirectory = System.getProperty("user.dir");
                    String programName = "";
                    String absoluteProgramPath = "";

                    if(System.getProperty("os.name").toUpperCase().indexOf("WIN") >= 0) { //windows configuration
                        programName = "open_graph.cmd";
                        absoluteProgramPath = workingDirectory + File.separator + "kernel" + File.separator + programName;
                        System.out.println("Final filepath : " + absoluteProgramPath);
                    }
                    if(System.getProperty("os.name").toUpperCase().indexOf("NUX") >= 0) { //linux configuration
                        programName = "open_graph.sh";
                        absoluteProgramPath = workingDirectory + File.separator + "kernel" + File.separator + programName;
                        System.out.println("Final filepath : " + absoluteProgramPath);
                    }
                    if(System.getProperty("os.name").toUpperCase().indexOf("MAC") >= 0) { //mac configuration
                        // do something here
                    }

                    // run
                    ProcessBuilder pb = new ProcessBuilder(absoluteProgramPath);
                    pb.directory(new File(workingDirectory + "/kernel"));
                    try{
                        Process process = pb.start();
                        //System.out.println("open_graph.cmd");
                    } catch (IOException e1) {
                        e1.printStackTrace();
                    }
                }
                else if (e.isShowGraphOrigClicked()) {
                    // this will happen when showGraphBtn was clicked
                    // shows graph
                    String workingDirectory = System.getProperty("user.dir");
                    String programName = "";
                    String absoluteProgramPath = "";

                    if(System.getProperty("os.name").toUpperCase().indexOf("WIN") >= 0) { //windows configuration
                        programName = "open_original_graph.cmd";
                        absoluteProgramPath = workingDirectory + File.separator + "kernel" + File.separator + programName;
                        System.out.println("Final filepath : " + absoluteProgramPath);
                    }
                    if(System.getProperty("os.name").toUpperCase().indexOf("NUX") >= 0) { //linux configuration
                        programName = "open_original_graph.sh";
                        absoluteProgramPath = workingDirectory + File.separator + "kernel" + File.separator + programName;
                        System.out.println("Final filepath : " + absoluteProgramPath);
                    }
                    if(System.getProperty("os.name").toUpperCase().indexOf("MAC") >= 0) { //mac configuration
                        // do something here
                    }

                    // run
                    ProcessBuilder pb = new ProcessBuilder(absoluteProgramPath);
                    pb.directory(new File(workingDirectory + "/kernel"));
                    try{
                        Process process = pb.start();
                        //System.out.println("open_original_graph.cmd");
                    } catch (IOException e1) {
                        e1.printStackTrace();
                    }
                }
            }
        });

        setMinimumSize(new Dimension(500, 600));
        setExtendedState(getExtendedState() | JFrame.MAXIMIZED_BOTH);

        // set what program should do when closing window
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        // set visible, so we can see it
        setVisible(true);
    }

    // creating MENUBAR
    private JMenuBar createMenuBar() {
        // new object - menubar
        JMenuBar menuBar = new JMenuBar();

        // new item in menubar - FILE
        JMenu fileMenu = new JMenu("File");
        // new files that will be in FILE
        JMenuItem loadSpectrumItem = new JMenuItem("Load Spectrum ...");
        JMenuItem loadResultsItem = new JMenuItem("Load Results...");
        JMenuItem loadGraphItem = new JMenuItem("Load Graph...");
        JMenuItem exitItem = new JMenuItem("Exit");

        // adding them in FILE
        fileMenu.add(loadSpectrumItem);
        fileMenu.addSeparator();
        fileMenu.add(loadResultsItem);
        fileMenu.add(loadGraphItem);
        fileMenu.addSeparator();
        fileMenu.add(exitItem);

        // new item in menubar - WINDOW
        JMenu editMenu = new JMenu("Edit");
        // new files that will be in FILE
        JMenuItem clearDataTableItem = new JMenuItem("Clear Database");
        JMenuItem clearResultsTableItem = new JMenuItem("Clear Results");
        JMenuItem clearStatusItem = new JMenuItem("Clear Status");
        // adding Items to WINDOW
        editMenu.add(clearDataTableItem);
        editMenu.add(clearResultsTableItem);
        editMenu.addSeparator();
        editMenu.add(clearStatusItem);


        // new item in menubar - HELP
        JMenu helpMenu = new JMenu("Help");
        // new files that will be in FILE
        JMenuItem updatesItem = new JMenuItem("Check for Updates");
        JMenuItem aboutItem = new JMenuItem("About");
        // adding Items to WINDOW
        helpMenu.add(updatesItem);
        helpMenu.add(aboutItem);

        // set up mnemonics
        fileMenu.setMnemonic(KeyEvent.VK_F);
        editMenu.setMnemonic(KeyEvent.VK_E);
        helpMenu.setMnemonic(KeyEvent.VK_H);
        exitItem.setMnemonic(KeyEvent.VK_X);

        // opens open dialog when clicking on load lights item in menubar
        loadSpectrumItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (configFileChooser.showOpenDialog(MainFrame.this) ==
                        JFileChooser.APPROVE_OPTION){
                    // make a copy of 'configFileChooser.getSelectedFile()' in input_data folder with name:
                    // experiment_data.txt

                    String workingDirectory = System.getProperty("user.dir");
                    String fileName = configFileChooser.getSelectedFile().getName();
                    String absoluteFilePath = "";

                    absoluteFilePath = workingDirectory + File.separator + "kernel" + File.separator +
                            "input_data" + File.separator + fileName;
                    System.out.println("Final filepath : " + absoluteFilePath);

                    InputStream is = null;
                    OutputStream os = null;
                    try {
                        is = new FileInputStream(configFileChooser.getSelectedFile());
                        textPanel.setExperimentSpectrum(configFileChooser.getSelectedFile().getName());
                        textPanel.clear();
                        textPanel.appendText("Loaded spectrum: \n" + textPanel.getExperimentSpectrum());
                        os = new FileOutputStream(absoluteFilePath);
                        byte[] buffer = new byte[1024];
                        int length;
                        while ((length = is.read(buffer)) > 0) {
                            os.write(buffer, 0, length);
                        }
                    } catch (FileNotFoundException ex) {
                        ex.printStackTrace();
                    } catch (IOException ex) {
                        ex.printStackTrace();
                    } finally {
                        try {
                            is.close();
                        } catch (IOException ex) {
                            ex.printStackTrace();
                        }
                        try {
                            os.close();
                        } catch (IOException ex) {
                            ex.printStackTrace();
                        }
                    }

                }
            }
        });

        // opens open dialog when clicking on load results item in menubar
        loadResultsItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (configFileChooser.showOpenDialog(MainFrame.this) ==
                        JFileChooser.APPROVE_OPTION){
                    controller.loadResultsFromFile(configFileChooser.getSelectedFile());
                    tablePanelOutput.refreshResults();
                }
            }
        });

        // opens open dialog when clicking on load graph item in menubar
        loadGraphItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (graphFileChooser.showOpenDialog(MainFrame.this) ==
                        JFileChooser.APPROVE_OPTION){
                    // shows graph in new frame
                    JFrame frame = new JFrame();
                    frame.setTitle("Graph");
                    ImageIcon icon = new ImageIcon(String.valueOf(graphFileChooser.getSelectedFile()));
                    JLabel label = new JLabel(icon);
                    frame.setExtendedState(getExtendedState() | JFrame.MAXIMIZED_BOTH);
                    label.setSize(icon.getIconWidth(), icon.getIconHeight());
                    frame.add(label);
                    frame.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
                    frame.pack();
                    frame.setVisible(true);
                }
            }
        });

        // exit button closes application
        exitItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                int action = showConfirmDialog(MainFrame.this,
                        "Do you really want to exit the application?",
                        "Confirm Exit", OK_CANCEL_OPTION);

                if (action == OK_OPTION) {
                    System.exit(0);
                }
            }
        });

        // clearDataTableItem Button clears upper table
        clearDataTableItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                tablePanelInput.clearData();
            }
        });

        // clearResultsTableItem Button clears lower table
        clearResultsTableItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                tablePanelOutput.clearResults();
            }
        });

        // clearBothTableItem Button clears both tables
        clearStatusItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                textPanel.clear();
            }
        });

        // opens message dialog when clicking on check for updates item in menubar
        updatesItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                JOptionPane.showMessageDialog(MainFrame.this,
                        "Your version is: 1.1\n" +
                                "The latest version is: 1.1",
                        "Update", JOptionPane.INFORMATION_MESSAGE);

            }
        });
        // opens message dialog when clicking on about item in menubar
        aboutItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                JOptionPane.showMessageDialog(MainFrame.this,
                        "Spectrum Analyzer\nVersion: 1.1\nAuthor: Ondrej Bily\nCopyright \u00a9 2019",
                        "About", JOptionPane.INFORMATION_MESSAGE);

            }
        });

        // set up accelerators
        exitItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, ActionEvent.CTRL_MASK));
        loadSpectrumItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_L, ActionEvent.CTRL_MASK));
        loadResultsItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_R, ActionEvent.CTRL_MASK));
        loadGraphItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, ActionEvent.CTRL_MASK));
        clearStatusItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q, ActionEvent.CTRL_MASK));

        // set up icons to menubar item
        exitItem.setIcon(createIcon("/images/Stop16.gif"));
        loadSpectrumItem.setIcon(createIcon("/images/Open16.gif"));
        loadResultsItem.setIcon(createIcon("/images/Open16.gif"));
        loadGraphItem.setIcon(createIcon("/images/Open16.gif"));
        clearStatusItem.setIcon(createIcon("/images/Delete16.gif"));
        clearResultsTableItem.setIcon(createIcon("/images/Delete16.gif"));
        clearDataTableItem.setIcon(createIcon("/images/Delete16.gif"));
        updatesItem.setIcon(createIcon("/images/Server16.gif"));
        aboutItem.setIcon(createIcon("/images/Information16.gif"));

        // adding FILE, WINDOW & HELP to menubar
        menuBar.add(fileMenu);
        menuBar.add(editMenu);
        menuBar.add(helpMenu);

        return menuBar;
    }

    private ImageIcon createIcon(String path) {
        URL url = getClass().getResource(path);

        if (url == null) {
            System.err.println("Unable to load image: " + path);
        }
        ImageIcon icon = new ImageIcon(url);

        return icon;
    }
}