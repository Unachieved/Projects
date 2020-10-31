package org.jondev.app;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Modality;
import javafx.stage.Stage;

/**
 * This components is used for the confirmation of a selection.
 */
public final class Confirm {

    private static final int INSET = 10;
    private static final int SPACING = 5;
    private static final int LAYOUT_HEIGHT = 80;
    private static final int LAYOUT_WIDTH = 200;

    private static Button noBtn;
    private static Button yesBtn;
    private static Boolean ret;

    /**
     * Constructor.
     */
    private Confirm() {
    }

    /**
     * This function sets up the displaying of of the confirmation dialog box.
     * @param message The question to be asked for confirmation
     * @return
     */
    public static Boolean display(String message) {

        Stage window = new Stage();
        window.initModality(Modality.APPLICATION_MODAL);

        Label label = new Label();
        label.setText(message);

        noBtn = new Button("no");
        yesBtn = new Button("yes");

        //if no clicked return to application
        //if yes clicked close application
        noBtn.setOnAction(e -> {
            ret = false;
            window.close();
        });
        yesBtn.setOnAction(e -> {
            ret = true;
            window.close();
        });

        //btn layout setip
        HBox btnLayout = new HBox();
        btnLayout.setAlignment(Pos.CENTER);
        btnLayout.getChildren().addAll(noBtn, yesBtn);
        btnLayout.setPadding(new Insets(INSET, INSET, INSET, INSET));
        btnLayout.setSpacing(SPACING);

        //overall popup layout setup
        VBox layout = new VBox();
        layout.getChildren().addAll(label, btnLayout);
        layout.setPadding(new Insets(INSET, INSET, INSET, INSET));
        layout.setSpacing(SPACING);
        layout.setAlignment(Pos.CENTER);

        Scene scene = new Scene(layout, LAYOUT_WIDTH, LAYOUT_HEIGHT);

        window.setScene(scene);
        window.showAndWait();

        return ret;
    }
}
