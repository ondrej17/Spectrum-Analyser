package gui.results_table;

import model.Result;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.List;

public class ResultTablePanel extends JPanel {

    private JTable table;
    private ResultTableModel resultTableModel;

    public ResultTablePanel() {

        resultTableModel = new ResultTableModel();
        table = new JTable(resultTableModel);

        setLayout(new BorderLayout());

        add(new JScrollPane(table), BorderLayout.CENTER);
    }

    public void setResults(List<Result> results) {
        resultTableModel.setResults(results);
    }

    public void refreshResults() {
        resultTableModel.fireTableDataChanged();
    }

    public void clearResults() {
        resultTableModel.clearResults();
        resultTableModel.fireTableDataChanged();
    }
}
