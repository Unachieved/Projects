package org.jondev.app;

/**
 * Data type containing the submitted data to be parsed as well as the parsed contact info.
 * n.b. only used for gui
 */
public class Entry {

    private String request;
    private String response;

    public Entry() {
        request = "";
        response = "";
    }

    public Entry(String req, String resp) {
        this.request = req;
        this.response = resp;
    }

    public String getRequest() {
        return this.request;
    }

    public void setRequest(String req) {
        this.request = req;
    }

    public String getResponse() {
        return this.response;
    }

    public void setResponse(String resp) {
        this.response = resp;
    }
}
