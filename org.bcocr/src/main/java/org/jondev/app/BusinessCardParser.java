package org.jondev.app;

import java.io.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Parses and filters data passed by the Business Card OCR allowing for the return
 * of a ContactInfo object.
 * @author Jon-Pierre
 * Specifications for all overridden methods can be found in BusinessCardParserInterface
 */
public class BusinessCardParser implements BusinessCardParserInterface {

    private static final int MAX_PHONE_LENGTH = 16;

    /**
     * view BusinessCardParserInterface.
     */
    @Override
    public ContactInfo getContactInfo(String document) {
        String[] parsedDoc = parseDocument(document);
        return new ContactInfo(parsedDoc[0], parsedDoc[1], parsedDoc[2]);
    }

    /**
     * filters contact information from data into corresponding array indices.
     * @param document: String containing the contact name, phone number and email to be
     *                     filtered from the data
     * @return: String array containing the contact name, phone number and email in that order
     */
    private String[] parseDocument(String document) {
        BufferedReader bufReader = new BufferedReader(new StringReader(document));

        //Regexes for filtering phone and email addresses from the data
        String phoneRegex = "(\\d\\W{0,4}\\d\\W{0,4}?\\d\\W{0,4}?\\d\\W{0,4}?\\d\\W{0,4}?\\d\\W{0,4}?)";
        String emailRegex = "[a-z0-9!#$%&'*+/=?^_‘{|}~-]+(?:.[a-z0-9!#$%&'*+/=?^_‘{|}~-]+)*@"
                    + "(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\z";

        //Stores the lines containing the corresponding variable names if found
        //otherwise remains and empty string
        String phoneLine = "";
        String emailLine = "";
        String nameLine = "";

        //document line iteration support variable
        String line = null;
        //the following try is place primarily as a way of removing the warning for
        //BufferedReader.readLine(): exception is not likely as a string is guaranteed to be passed in
        try {
            while ((line = bufReader.readLine()) != null) {
                //phone number and email matchers for recognition in data line
                Matcher phoneMatcher = Pattern.compile(phoneRegex).matcher(line);
                Matcher emailMatcher = Pattern.compile(emailRegex).matcher(line);

                // check if line contains contact phone number
                // else check if line contains contact email address
                if (phoneMatcher.find() && !line.toLowerCase().contains("fax")) {
                    phoneLine = line;
                } else if (emailMatcher.find()) {
                    emailLine = line.substring(emailMatcher.start(), emailMatcher.end());
                }
            }

            //the existence of a business email is required for the identification of a contacts name
            //see SRS for explanation
            if (emailLine.length() > 0) {
                bufReader = new BufferedReader(new StringReader(document));
                //get partial contact name from the first portion of the email
                //before the "@" sign
                String namePartial = emailLine.substring(0, emailLine.indexOf("@"));

                //while iterating through each line of the document
                while ((line = bufReader.readLine()) != null) {

                    //check each independent word (denoted by " " separation) to see
                    //if it matches the name in the business email. it if does then the
                    //corresponding line contains the contact name
                    String[] maybeNames = line.split(" ");
                    boolean multiNameInstance = false;
                    for (int i = 0; i < maybeNames.length; i++) {

                        if (namePartial.toLowerCase().contains(maybeNames[i].toLowerCase()) && nameLine.length() < 1) {
                            nameLine = line;
                            break;
                        } else if (namePartial.toLowerCase().contains(maybeNames[i].toLowerCase())
                            && nameLine.length() > 1) {

                            //if a match was already found for the contact name, then there
                            //occurs 2 string that could be contact name: contact name cannot
                            //be determined
                            nameLine = "";
                            multiNameInstance = true;
                            break;
                        }
                    }
                    if (multiNameInstance) {
                        break;
                    }
                }
            }

        } catch (IOException e) {
            System.out.println("A problem occured while trying to parse name. Try again");
        }

        //the final phone string from the line containing the phone number
        //after removing all non digit characters
        String phone = phoneLine.replaceAll("\\D+", "");

        //phone standard is based on E.164 which states phone numbers are
        //a maximum of length 15
        if (phone.length() > MAX_PHONE_LENGTH) {
            phone = "";
        }

        return new String[]{nameLine, phone, emailLine};
    }
}
