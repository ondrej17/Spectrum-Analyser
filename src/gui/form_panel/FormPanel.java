package gui.form_panel;

import javax.swing.*;
import javax.swing.border.TitledBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.net.URL;
import java.util.Objects;

public class FormPanel extends JPanel {

    // components of gui.form_panel.FormPanel
    private JLabel minLambdaLabel;
    private JLabel maxLambdaLabel;
    //private JLabel angExpLabel;
    private JLabel deltaLambdaLabel;
    private JTextField minLambdaField;
    private JTextField maxLambdaField;
    //private JTextField angExpField;
    private JTextField deltaLambdaField;
    private JComboBox database;
    /*private JRadioButton oneLightsRadio;
    private JRadioButton genericLightsRadio;
    private JRadioButton allLightsRadio;
    private JRadioButton customLightsRadio;
    private ButtonGroup inputFileGroup;*/

    private JButton doItForMeBtn;
    private JButton startBtn;
    private JButton showGraphBtn;
    private JButton showGraphOrigBtn;

    private FormListener formListener;

    // constructor of gui.form_panel.FormPanel
    public FormPanel() {
        // find preferred dimension of FromPanel
        Dimension dim = getPreferredSize();

        // set width of gui.form_panel.FormPanel
        dim.width = 250;

        // set a new width
        setPreferredSize(dim);

        // set components
        minLambdaLabel = new JLabel("\u03BB\u2098\u1D62\u2099: ");
        maxLambdaLabel = new JLabel("\u03BB\u2098\u2090\u2093: ");
        //angExpLabel = new JLabel("\u03B1: ");
        deltaLambdaLabel = new JLabel("\u0394\u03BB: ");

        minLambdaField = new JTextField(10);
        maxLambdaField = new JTextField(10);
        //angExpField = new JTextField(10);
        deltaLambdaField = new JTextField(10);

        database = new JComboBox();
        /*oneLightsRadio = new JRadioButton("One of Kind");
        genericLightsRadio = new JRadioButton("Generic Spectra");
        allLightsRadio = new JRadioButton("All of them");
        customLightsRadio = new JRadioButton("Custom Selection");
        inputFileGroup = new ButtonGroup();*/

        // set up input file radios
        /*inputFileGroup.add(genericLightsRadio);
        inputFileGroup.add(oneLightsRadio);
        inputFileGroup.add(allLightsRadio);
        inputFileGroup.add(customLightsRadio);*/
        DefaultComboBoxModel databaseModel = new DefaultComboBoxModel();
        databaseModel.addElement("list_of_lights_generic.txt");
        databaseModel.addElement("list_of_lights_one.txt");
        databaseModel.addElement("list_of_lights_all.txt");
        databaseModel.addElement("list_of_lights_custom.txt");
        database.setModel(databaseModel);

        /*oneLightsRadio.setActionCommand("list_of_lights_one.txt");
        genericLightsRadio.setActionCommand("list_of_lights_generic.txt");
        allLightsRadio.setActionCommand("list_of_lights_all.txt");
        customLightsRadio.setActionCommand("list_of_lights_custom.txt");*/

        // set up buttons
        doItForMeBtn = new JButton("Find Optimum");
        startBtn = new JButton("Run");
        showGraphBtn = new JButton("Plot INT");
        showGraphOrigBtn = new JButton("Plot NON");

        // set mnemonics
        doItForMeBtn.setMnemonic(KeyEvent.VK_D);
        startBtn.setMnemonic(KeyEvent.VK_R);
        showGraphBtn.setMnemonic(KeyEvent.VK_A);
        showGraphOrigBtn.setMnemonic(KeyEvent.VK_S);

        // set tool tip for button
        doItForMeBtn.setToolTipText("Runs program for every combination of selected lights");
        startBtn.setToolTipText("Starts external program that calculates percentage of each light");
        showGraphBtn.setToolTipText("Shows comparison graph with both experiment and calculated spectra (integrated)");
        showGraphOrigBtn.setToolTipText("Shows comparison graph with both experiment and calculated spectra (non-integrated)");

        // default values in fields
        minLambdaField.setText("400");
        maxLambdaField.setText("700");
        deltaLambdaField.setText("20");
        database.setSelectedIndex(0);

        // set borders - inner and outer
        TitledBorder innerBorder = BorderFactory.createTitledBorder("Set parameters");
        javax.swing.border.Border outerBorder = BorderFactory.createEmptyBorder(5, 5, 5, 5);
        setBorder(BorderFactory.createCompoundBorder(outerBorder, innerBorder));

        // set up icons for buttons
        doItForMeBtn.setIcon(createIcon("/images/Edit16.gif"));
        startBtn.setIcon(createIcon("/images/Play16.gif"));
        showGraphBtn.setIcon(createIcon("/images/Find16.gif"));
        showGraphOrigBtn.setIcon(createIcon("/images/Find16.gif"));


        database.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String minLambda = minLambdaField.getText();
                String maxLambda = maxLambdaField.getText();
                //String angExp = angExpField.getText();
                String deltaLambda = deltaLambdaField.getText();
                String inputFile = (String) database.getSelectedItem();
                System.out.println("inputFile: " + inputFile);

                boolean inputFileChanged = true;
                boolean showLightsClicked = false;
                boolean startClicked = false;
                boolean showGraphClicked = false;
                boolean showGraphOrigClicked = false;

                FormEvent ev = new FormEvent(this, minLambda, maxLambda, /*angExp,*/ deltaLambda, inputFile,
                        inputFileChanged, showLightsClicked, startClicked, showGraphClicked, showGraphOrigClicked);

                if (formListener != null){
                    formListener.formEventOccurred(ev);
                }
            }
        });

        // set up buttons' actions
        doItForMeBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String minLambda = minLambdaField.getText();
                String maxLambda = maxLambdaField.getText();
                String deltaLambda = deltaLambdaField.getText();
                String inputFile = (String) database.getSelectedItem();
                System.out.println("inputFile: " + inputFile);

                boolean inputFileChanged = false;
                boolean doItForMeBtnClicked = true;
                boolean startClicked = false;
                boolean showGraphClicked = false;
                boolean showGraphOrigClicked = false;

                FormEvent ev = new FormEvent(this, minLambda, maxLambda, /*angExp,*/ deltaLambda, inputFile,
                        inputFileChanged, doItForMeBtnClicked, startClicked, showGraphClicked, showGraphOrigClicked);

                if (formListener != null){
                    formListener.formEventOccurred(ev);
                }
            }
        });
        startBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String minLambda = minLambdaField.getText();
                String maxLambda = maxLambdaField.getText();
                //String angExp = angExpField.getText();
                String deltaLambda = deltaLambdaField.getText();
                String inputFile = (String) database.getSelectedItem();
                System.out.println("inputFile: " + inputFile);

                boolean inputFileChanged = false;
                boolean doItForMeBtnClicked = false;
                boolean startClicked = true;
                boolean showGraphClicked = false;
                boolean showGraphOrigClicked = false;

                FormEvent ev = new FormEvent(this, minLambda, maxLambda, /*angExp,*/ deltaLambda, inputFile,
                        inputFileChanged, doItForMeBtnClicked, startClicked, showGraphClicked, showGraphOrigClicked);

                if (formListener != null){
                    formListener.formEventOccurred(ev);
                }
            }
        });
        showGraphBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String minLambda = minLambdaField.getText();
                String maxLambda = maxLambdaField.getText();
                //String angExp = angExpField.getText();
                String deltaLambda = deltaLambdaField.getText();
                String inputFile = (String) database.getSelectedItem();
                System.out.println("inputFile: " + inputFile);

                boolean inputFileChanged = false;
                boolean doItForMeBtnClicked = false;
                boolean startClicked = false;
                boolean showGraphClicked = true;
                boolean showGraphOrigClicked = false;

                FormEvent ev = new FormEvent(this, minLambda, maxLambda, /*angExp,*/ deltaLambda, inputFile,
                        inputFileChanged, doItForMeBtnClicked, startClicked, showGraphClicked, showGraphOrigClicked);

                if (formListener != null){
                    formListener.formEventOccurred(ev);
                }
            }
        });
        showGraphOrigBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String minLambda = minLambdaField.getText();
                String maxLambda = maxLambdaField.getText();
                //String angExp = angExpField.getText();
                String deltaLambda = deltaLambdaField.getText();
                String inputFile = (String) database.getSelectedItem();
                System.out.println("inputFile: " + inputFile);

                boolean inputFileChanged = false;
                boolean doItForMeBtnClicked = false;
                boolean startClicked = false;
                boolean showGraphClicked = false;
                boolean showGraphOrigClicked = true;

                FormEvent ev = new FormEvent(this, minLambda, maxLambda, /*angExp,*/ deltaLambda, inputFile,
                        inputFileChanged, doItForMeBtnClicked, startClicked, showGraphClicked, showGraphOrigClicked);

                if (formListener != null){
                    formListener.formEventOccurred(ev);
                }
            }
        });

        // set layout and add components
        layoutComponents();

    }

    private ImageIcon createIcon(String path) {
        URL url = getClass().getResource(path);

        if (url == null) {
            System.err.println("Unable to load image: " + path);
        }
        ImageIcon icon = new ImageIcon(url);

        return icon;
    }

    public void setFormListener(FormListener listener){
        this.formListener = listener;
    }

    public void layoutComponents() {
        // set layout
        setLayout(new GridBagLayout());

        // create an important object - gc
        GridBagConstraints gc = new GridBagConstraints();

        // something that needs to be done
        gc.fill = GridBagConstraints.NONE;

        // 1 ROW
        gc.gridy = 0;
        gc.weightx = 1;
        gc.weighty = 0.1;

            // add minLambdaLabel at [0, 0]
            gc.gridx = 0;
            gc.anchor = GridBagConstraints.LINE_END;
            gc.insets = new Insets(0, 0, 0, 5);
            add(minLambdaLabel, gc);

            // add minLambdaField at [1, 0]
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.LINE_START;
            gc.insets = new Insets(0, 0, 0, 0 );
            add(minLambdaField, gc);

        // 2 ROW
        gc.gridy++;
        gc.weightx = 1;
        gc.weighty = 0.1;

            // add maxLambdaLabel at [0, 1]
            gc.gridx = 0;
            gc.anchor = GridBagConstraints.LINE_END;
            gc.insets = new Insets(0, 0, 0, 5);
            add(maxLambdaLabel, gc);

            // add maxLambdaField at [1, 1]
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.LINE_START;
            gc.insets = new Insets(0, 0, 0, 0 );
            add(maxLambdaField, gc);

        // 3 ROW
        /*gc.gridy++;
        gc.weightx = 1;
        gc.weighty = 0.1;

            // add angExpLabel at [0, 2]
            gc.gridx = 0;
            gc.anchor = GridBagConstraints.LINE_END;
            gc.insets = new Insets(0, 0, 0, 5);
            add(angExpLabel, gc);

            // add angExpField at [1, 2]
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.LINE_START;
            gc.insets = new Insets(0, 0, 0, 0 );
            add(angExpField, gc);*/

        // 4 ROW
        gc.gridy++;
        gc.weightx = 1;
        gc.weighty = 0.1;

            // add deltaLambdaLabel at [0, 3]
            gc.gridx = 0;
            gc.anchor = GridBagConstraints.LINE_END;
            gc.insets = new Insets(0, 0, 0, 5);
            add(deltaLambdaLabel, gc);

            // add deltaLambdaField at [1, 3]
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.LINE_START;
            gc.insets = new Insets(0, 0, 0, 0 );
            add(deltaLambdaField, gc);

        // 5-1 ROW
        gc.gridy++;
        gc.weightx = 1;
        gc.weighty = 0.1;

            // add inputFile label at [0, 3]
            gc.gridx = 0;
            gc.insets = new Insets(0, 0, 0, 5);
            gc.anchor = GridBagConstraints.LINE_END;
            add(new JLabel("Database: "), gc);

            // add oneLightsRadio at [1, 2]
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.LINE_START;
            gc.insets = new Insets(0, 0, 0, 0);
            add(database, gc);

        /*// 6 ROW
        gc.gridy++;
        gc.weightx = 1;
        gc.weighty = 0.1;

            // add inputFile label at [0, 3]
            gc.gridx = 0;
            gc.insets = new Insets(0, 0, 0, 5);
            gc.anchor = GridBagConstraints.LINE_END;
            add(new JLabel("Preset: "), gc);

            // add oneLightsRadio at [1, 2]
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.FIRST_LINE_START;
            gc.insets = new Insets(0, 0, 0, 0);
            add(genericLightsRadio, gc);

        // 7 ROW
        gc.gridy++;

        gc.weightx = 1;
        gc.weighty = 0.1;

            // add empCombo at [1, 2]
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.FIRST_LINE_START;
            gc.insets = new Insets(0, 0, 0, 0);
            add(oneLightsRadio, gc);

        // 9 ROW
        gc.gridy++;

        gc.weightx = 1;
        gc.weighty = 0.1;

            // add empCombo at [1, 2]
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.FIRST_LINE_START;
            gc.insets = new Insets(0, 0, 0, 0);
            add(allLightsRadio, gc);

        // 10 ROW
        gc.gridy++;

        gc.weightx = 1;
        gc.weighty = 0.2;

            // add empCombo at [1, 2]
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.FIRST_LINE_START;
            gc.insets = new Insets(0, 0, 0, 0);
            add(customLightsRadio, gc);*/

        // Buttons
        Dimension btnSize = doItForMeBtn.getPreferredSize();
        doItForMeBtn.setPreferredSize(btnSize);
        startBtn.setPreferredSize(btnSize);
        showGraphBtn.setPreferredSize(btnSize);
        showGraphOrigBtn.setPreferredSize(btnSize);

        // 11 ROW
        gc.gridy++;
        gc.weightx = 1;
        gc.weighty = 0.2;

            // add showLightsBtn at [0, 2]
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.LINE_START;
            gc.insets = new Insets(0, 5, 0, 0);
            add(doItForMeBtn, gc);

        // 12 ROW
        gc.gridy++;
        gc.weightx = 1;
        gc.weighty = 0.2;

            // add startBtn
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.LINE_START;
            gc.insets = new Insets(0, 5, 0, 0);
            add(startBtn, gc);

        // 14 ROW
        gc.gridy++;
        gc.weightx = 1;
        gc.weighty = 0.2;

            // add showGraphBtn
            gc.gridx = 1;
            gc.anchor = GridBagConstraints.LINE_START;
            gc.insets = new Insets(0, 5, 0, 0);
            add(showGraphBtn, gc);

        // 15 ROW
        gc.gridy++;
        gc.weightx = 1;
        gc.weighty = 0.2;

        // add showGraphBtn
        gc.gridx = 1;
        gc.anchor = GridBagConstraints.LINE_START;
        gc.insets = new Insets(0, 5, 0, 0);
        add(showGraphOrigBtn, gc);
    }
}
