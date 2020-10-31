package org.jondev.app;

/**
 * Interface for implementation of ContactInfo.
 */
interface ContactInfoInterface {
    /**
     * @return
     *      the full name of the individual (eg. John Smith, Susan Malick)
     */
    String getName();

    /**
     * @return
     *      the phone number formatted as a sequence of digits
     */
    String getPhoneNumber();

    /**
     * @return
     *      the phone number formatted as a sequence of digits
     */
    String getEmailAddress();
}
