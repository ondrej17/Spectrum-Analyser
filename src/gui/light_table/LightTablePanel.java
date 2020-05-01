package gui.light_table;

import gui.form_panel.FormListener;
import model.Light;

import javax.swing.*;
import java.awt.*;
import java.util.List;


public class LightTablePanel extends JPanel {

    private JTable table;
    private LightTableModel dataTableModel;

    public LightTablePanel() {

        dataTableModel = new LightTableModel();
        table = new JTable(dataTableModel);

        setLayout(new BorderLayout());

        add(new JScrollPane(table), BorderLayout.CENTER);
    }

    public void setData(List<Light> lights) {
        dataTableModel.setData(lights);
    }

    public void refreshData() {
        dataTableModel.fireTableDataChanged();
    }

    public void clearData() {
        dataTableModel.clearData();
        dataTableModel.fireTableDataChanged();
    }
}
