package org.jondev.app;

import javafx.application.Application;
import javafx.beans.binding.Bindings;
import javafx.collections.ObservableList;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

/**
 * The standalone gui to be displayed when the program is ran. A gui interface
 * is presented instead of having to use the command line
 */
public class Guifx extends Application {

    private static final int INSET = 10;
    private static final int SPACING = 5;
    private static final int MIN_COL_WIDTH = 150;
    private static final int WINDOW_HEIGHT = 400;
    private static final int WINDOW_WIDTH = 300;
    private static final Label TABLE_LABEL = new Label("Data Table");

    private Stage window;
    private Button addContact;
    private Button remove;
    private TableView<Entry> table;

    public Guifx() {
    }

    /**
     * Primary setup method for the functioning of the gui.
     */
    private void run() {

        //button inits
        addContact = new Button("Add Contact");
        remove = new Button("Remove Entry");

        //setup table columns
        TableColumn<Entry, String> reqCol = new TableColumn<>("Request");
        reqCol.setMinWidth(MIN_COL_WIDTH);
        reqCol.setCellValueFactory(new PropertyValueFactory<>("request"));

        TableColumn<Entry, String> resCol = new TableColumn<>("Response");
        resCol.setMinWidth(MIN_COL_WIDTH);
        resCol.setCellValueFactory(new PropertyValueFactory<>("response"));

        //setup table
        table = new TableView<>();
        table.getColumns().setAll(reqCol, resCol);
        table.prefWidthProperty().bind(window.widthProperty());
        table.setColumnResizePolicy(TableView.CONSTRAINED_RESIZE_POLICY);

        //add button setup.
        //On click display popup windown for data entry. On data submission, format
        //and add the request along side the contact information to the table
        addContact.setOnAction(e -> {

            //get the data that had been entered
            String tempData = DataIn.display();
            DataIn.clear();
            StringBuilder resp = new StringBuilder();
            if (tempData.length() > 0) {

                //get the contact information using the parser, followed by formatting
                ContactInfo tmp = new BusinessCardParser().getContactInfo(tempData);
                resp.append("Name: " + tmp.getName() + "\n");
                resp.append("Phone: " + tmp.getPhoneNumber() + "\n");
                resp.append("Email: " + tmp.getEmailAddress() + "\n");

                //create and add entry to the table
                Entry entry = new Entry(tempData, resp.toString());
                table.getItems().add(entry);
            }
        });
        //remove button setup
        //On click remove selected entry from the table
        remove.setOnAction(e -> {

            ObservableList<Entry> entries = table.getItems();
            ObservableList<Entry> entrySelected = table.getSelectionModel().getSelectedItems();

            for (int i = 0; i < entrySelected.size(); i++) {
                entries.remove(entrySelected.get(i));
            }
        });
        //disable remove if table is empty
        remove.disableProperty().bind(Bindings.size(table.getItems()).isEqualTo(0));

        //button layout
        HBox buttonLayout = new HBox();
        buttonLayout.setPadding(new Insets(INSET, INSET, INSET, INSET));
        buttonLayout.setSpacing(SPACING);
        buttonLayout.getChildren().addAll(addContact, remove);

        //general gui layout
        VBox tableLayout = new VBox();
        tableLayout.setPadding(new Insets(INSET, INSET, INSET, INSET));
        tableLayout.setSpacing(SPACING);
        tableLayout.getChildren().addAll(TABLE_LABEL, table, buttonLayout);

        //scene setup
        Scene scene = new Scene(tableLayout, WINDOW_WIDTH, WINDOW_HEIGHT);

        //window setup
        window.setTitle("Business Card Parser");
        window.setScene(scene);
        window.setOnCloseRequest(e -> {
            e.consume();
            Boolean close = Confirm.display("Are you sure you wish to exit?");
            if (close) {
                window.close();
            }
        });
        window.show();
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        window = primaryStage;
        run();
    }
}
