package gui;

import javax.swing.filechooser.FileFilter;
import java.io.File;

public class GraphFileFilter extends FileFilter {
    @Override
    public boolean accept(File file) {

        if (file.isDirectory()){
            return true;
        }

        String name = file.getName();

        String extension = Utils.getFileExtension(name);
        if (extension == null){
            return false;
        }

        if (extension.equals("jpg") || extension.equals("jpeg")
                || extension.equals("png")){
            return true;
        }

        return false;
    }

    @Override
    public String getDescription() {
        return "Graph (*.jpg, *.jpeg, *.png)";
    }
}
