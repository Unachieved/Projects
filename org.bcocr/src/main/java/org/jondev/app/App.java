package org.jondev.app;

import javafx.application.Application;

/**
 * Application entry point.
 */
public final class App {
    private App() {
    }

    /**
     * @param args The arguments of the program.
     */
    public static void main(String[] args) {
        Application.launch(Guifx.class, args);
    }
}

