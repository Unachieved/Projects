package org.jondev.app;

import javafx.beans.binding.Bindings;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Modality;
import javafx.stage.Stage;

/**
 * Used for adding data to the gui table via a popup window.
 */
public final class DataIn {

    private static final int INSET = 10;
    private static final int SPACING = 5;
    private static final int LAYOUT_HEIGHT = 200;
    private static final int LAYOUT_WIDTH = 300;

    private static String ret = "";

    private DataIn() {
    }

    /**
     * Displays a whindow when the user wants to add a new entry to the table.
     * @return the data entered to be parsed for contact information
     */
    public static String display() {

        //init window && force submission or cancelation;
        Stage window = new Stage();
        window.initModality(Modality.APPLICATION_MODAL);
        window.setTitle("Add Popup");

        //initialize fields used in window
        Label label = new Label();
        TextArea input = new TextArea();
        Button submit = new Button("Submit");
        Button cancel = new Button("Cancel");

        //textarea and label setup
        label.setText("Enter Business Card Data:");
        input.setPromptText("Enter Business Card Data:");

        //on submit set ret to be the String data entered
        submit.setOnAction(e -> {
            ret = input.getText();
            window.close();
        });
        //disable submit if no data entered
        submit.disableProperty().bind(Bindings.isEmpty(input.textProperty()));
        //close window on cancel
        cancel.setOnAction(e -> {
            window.close();
        });

        //button layout setup
        HBox btnLayout = new HBox();
        btnLayout.getChildren().addAll(cancel, submit);
        btnLayout.setSpacing(SPACING);

        //general gui layout
        VBox layout = new VBox();
        layout.getChildren().addAll(label, input, btnLayout);
        layout.setPadding(new Insets(INSET, INSET, INSET, INSET));
        layout.setSpacing(SPACING);

        //add scene required by javafx
        Scene scene = new Scene(layout, LAYOUT_WIDTH, LAYOUT_HEIGHT);

        window.setScene(scene);
        window.showAndWait();

        return ret;
    }

    /**
     * clear the ret string after it has been set in submission.
     */
    public static void clear() {
        ret = "";
    }
}
