package org.jondev.app;

/**
 * @author Jon-Pierre
 * The follow class contains variables to be used for custom testing purposes
 * The variables come in pairs of the format: "(.+In[0-9]+)", "(.+Out[0-9]+)"
 */

public final class CustomVars {

    private static final int TVAL3 = 3;
    private static final int TVAL4 = 4;
    private static final int TVAL5 = 5;

    //pair 1
    private static String in1 = "custname@custcompany.com";
    private static String out1 = "Name: \n"
                + "Phone: \n"
                + "Email: custname@custcompany.com";

    //pair 2
    private static String in2 = "1234567890";
    private static String out2 = "Name: \n"
                + "Phone: 1234567890\n"
                + "Email: ";

    //pair 3
    private static String in3 = "custname@custcompany.com\n"
                + "1234567890\n"
                + "Invalid Person";
    private static String out3 = "Name: \n"
                + "Phone: 1234567890\n"
                + "Email: custname@custcompany.com";

    //pair 4
    private static String in4 = "custname@custcompany.com";
    private static String out4 = "Name: \n"
                + "Phone: \n"
                + "Email: custname@custcompany.com";

    //pair 5
    private static String in5 = "custname@custcompany.com\n"
                + "1234567890\n"
                + "Cust Name\n"
                + "Company Name";
    private static String out5 = "Name: \n"
                + "Phone: 1234567890\n"
                + "Email: custname@custcompany.com";

    //pair 6
    private static String in6 = "";
    private static String out6 = "Name: \n"
                + "Phone: \n"
                + "Email: ";

    private CustomVars() {

    }

    /**
     * Method to get the requested test string based on params.
     * @param test
     *      test number being ran
     * @param type
     *      denote which string is being requested: input or output
     * @return  requested test string
     */
    public static String getVar(Integer test, Integer type) {
        switch (test) {
            case 0:
                return type == 0 ? in1 : out1;
            case 1:
                return type == 0 ? in2 : out2;
            case 2:
                return type == 0 ? in3 : out3;
            case TVAL3:
                return type == 0 ? in4 : out4;
            case TVAL4:
                return type == 0 ? in5 : out5;
            case TVAL5:
                return type == 0 ? in6 : out6;
            default:
                return type == 0 ? "" : out6;
        }
    }
}
