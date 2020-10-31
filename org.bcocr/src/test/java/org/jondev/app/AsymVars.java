package org.jondev.app;

/**
 * @author Jon-Pierre
 * The follow class contains variables to be used for Asymmetrik testing purposes
 * The variables come in pairs of the format: "(.+In[0-9]+)", "(.+Out[0-9]+)"
 */

public final class AsymVars {

    //pair 1
    private static String asymIn1 = "ASYMMETRIK LTD\n"
                + "Mike Smith\n"
                + "Senior Software Engineer\n"
                + "(410)555-1234\n"
                + "msmith@asymmetrik.com\n";
    private static String asymOut1 = "Name: Mike Smith\n"
                + "Phone: 4105551234\n"
                + "Email: msmith@asymmetrik.com";

    //pair 2
    private static String asymIn2 = "Foobar Technologies\n"
            + "Analytic Developer\n"
            + "Lisa Haung\n"
            + "1234 Sentry Road\n"
            + "Columbia, MD 12345\n"
            + "Phone: 410-555-1234\n"
            + "Fax: 410-555-4321\n"
            + "lisa.haung@foobartech.com";
    private static String asymOut2 = "Name: Lisa Haung\n"
            + "Phone: 4105551234\n"
            + "Email: lisa.haung@foobartech.com";

    //pair 3
    private static String asymIn3 = "Arthur Wilson\n"
            + "Software Engineer\n"
            + "Decision & Security Technologies\n"
            + "ABC Technologies\n"
            + "123 North 11th Street\n"
            + "Suite 229\n"
            + "Arlington, VA 22209\n"
            + "Tel: +1 (703) 555-1259\n"
            + "Fax: +1 (703) 555-1200\n"
            + "awilson@abctech.com";
    private static String asymOut3 = "Name: Arthur Wilson\n"
            + "Phone: 17035551259\n"
            + "Email: awilson@abctech.com";

    private AsymVars() {

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
                return type == 0 ? asymIn1 : asymOut1;
            case 1:
                return type == 0 ? asymIn2 : asymOut2;
            case 2:
                return type == 0 ? asymIn3 : asymOut3;
            default:
                return "";
        }
    }
}
