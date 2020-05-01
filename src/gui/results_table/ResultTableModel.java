package gui.results_table;

import model.Result;

import javax.swing.table.AbstractTableModel;
import java.util.List;

public class ResultTableModel extends AbstractTableModel {

    private List<Result> results;

    private String[] colNames =  {"Name", "Minimum [%]", "Middle [%]", "Maximum [%]"};

    @Override
    public String getColumnName(int column) {
        return colNames[column];
    }

    @Override
    public int getRowCount() {
        return results.size();
    }

    @Override
    public int getColumnCount() {
        return 4;
    }

    @Override
    public Object getValueAt(int row, int col) {

        Result result = results.get(row);

        switch (col) {
            case 0:
                return result.getName();
            case 1:
                return result.getMinimum();
            case 2:
                return result.getMiddle();
            case 3:
                return result.getMaximum();
        }
        return null;
    }

    public void setResults(List<Result> results) {
        this.results = results;
    }

    public void clearResults() {
        results.removeAll(results);
    }

}
