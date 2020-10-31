package org.jondev.app;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

/**
 * The following are test for the BusinessCard_OCR parser component.
 * These specific test cases were provided by Asymmetrik. Each case pair
 * can be found in the file TestVars.java
 * @author Jon-Pierre
 *
 */
public class AsymTests {

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
    public void asymTest1() {
        ContactInfo tester = new BusinessCardParser().getContactInfo(AsymVars.getVar(0, 0));
        assertEquals(AsymVars.getVar(0, 1), genOutput(tester));
    }

    @Test
    public void asymTest2() {
        ContactInfo tester = new BusinessCardParser().getContactInfo(AsymVars.getVar(1, 0));
        assertEquals(AsymVars.getVar(1, 1), genOutput(tester));
    }

    @Test
    public void asymTest3() {
        ContactInfo tester = new BusinessCardParser().getContactInfo(AsymVars.getVar(2, 0));
        assertEquals(AsymVars.getVar(2, 1), genOutput(tester));
    }
}
