package gui.light_table;

import model.Light;

import javax.swing.table.AbstractTableModel;
import java.util.List;

public class LightTableModel extends AbstractTableModel {

    private List<Light> lights;

    private String[] colNames =  {"Name", "Participation"};

    @Override
    public String getColumnName(int column) {
        return colNames[column];
    }

    @Override
    public int getRowCount() {
        return lights.size();
    }

    @Override
    public int getColumnCount() {
        return 2;
    }

    @Override
    public Object getValueAt(int row, int col) {

        Light light = lights.get(row);

        switch (col) {
            case 0:
                return light.getName();
            case 1:
                return light.getParticipation();
        }
        return null;
    }

    @Override
    public boolean isCellEditable(int rowIndex, int columnIndex)
    {
        if (columnIndex == 1) {
            return true;
        }
        else {
            return false;
        }
    }

    @Override
    public Class<?> getColumnClass(int col) {
        switch (col) {
            case 0:
                return String.class;
            case 1:
                return Boolean.class;
            default:
                return null;
        }
    }

    @Override
    public void setValueAt(Object value, int row, int col)
    {
        if (lights == null) return;

        Light light = lights.get(row);

        switch (col) {
            case 1:
                light.setParticipation((Boolean) value);
                break;
            default:
                return;

        }
    }

    public void setData(List<Light> lights) {
        this.lights = lights;
    }

    public void clearData() {
        lights.removeAll(lights);
    }
}
