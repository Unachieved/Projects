package org.jondev.app;

/**
 * Implementation of the ContactInfoInterface.
 * @author Jon-Pierre
 * Specifications for all overridden methods can be found in ContactInfoInterface
 */
public class ContactInfo implements ContactInfoInterface {

    //Contact Information
    private String name;
    private String phoneNumber;
    private String emailAddress;

    /**
     * Constructs the ContactInfo object using the 3 required parameters.
     * @param name
     *      Contact name
     * @param phone
     *      Contact phone number
     * @param email
     *      Contact email address
     */
    public ContactInfo(String name, String phone, String email) {
        this.name = name;
        phoneNumber = phone;
        emailAddress = email;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getPhoneNumber() {
        return phoneNumber;
    }

    @Override
    public String getEmailAddress() {
        return emailAddress;
    }
}
