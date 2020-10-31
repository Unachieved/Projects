package org.jondev.app;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

/**
 * The following are test for the BusinessCard_OCR parser component.
 * These specific test cases were custom and test cases as defined test names.
 * Each case pair can be found in the file CustomVars.java
 * @author Jon-Pierre
 *
 */
public class CustomTests {

    private static int tVal1 = 1;
    private static int tVal2 = 2;
    private static int tVal3 = 3;
    private static int tVal4 = 4;
    private static int tVal5 = 5;

    /**
     * Utility generator for the final output of the businesscard parser.
     * @param tmp
     *         The contact info object to be accessed for output information
     * @return the output string in the requested format
     */
    public String genOutput(ContactInfo tmp) {
        return "Name: " + tmp.getName() + "\n"
                + "Phone: " + tmp.getPhoneNumber() + "\n"
                + "Email: " + tmp.getEmailAddress();
    }

    @Test
    public void shouldReturnEmailOnly() {
        ContactInfo tester = new BusinessCardParser().getContactInfo(CustomVars.getVar(0, 0));
        assertEquals(CustomVars.getVar(0, 1), genOutput(tester));
    }

    @Test
    public void shouldReturnPhoneNumberOnly() {
        ContactInfo tester = new BusinessCardParser().getContactInfo(CustomVars.getVar(tVal1, 0));
        assertEquals(CustomVars.getVar(tVal1, 1), genOutput(tester));
    }

    @Test
    public void shouldNotReturnName() {
        ContactInfo tester = new BusinessCardParser().getContactInfo(CustomVars.getVar(tVal2, 0));
        assertEquals(CustomVars.getVar(tVal2, 1), genOutput(tester));
    }

    @Test
    public void shouldNotReturnNameAndEmail() {
        ContactInfo tester = new BusinessCardParser().getContactInfo(CustomVars.getVar(tVal3, 0));
        assertEquals(CustomVars.getVar(tVal3, 1), genOutput(tester));
    }

    @Test
    public void businessNameLikeContactName() {
        ContactInfo tester = new BusinessCardParser().getContactInfo(CustomVars.getVar(tVal4, 0));
        assertEquals(CustomVars.getVar(tVal4, 1), genOutput(tester));
    }

    @Test
    public void noParserInput() {
        ContactInfo tester = new BusinessCardParser().getContactInfo(CustomVars.getVar(tVal5, 0));
        assertEquals(CustomVars.getVar(tVal5, 1), genOutput(tester));
    }
}
