package org.jondev.app;

/**
 * BusinessCardParsers Interface to aid creation of class.
 */
public interface BusinessCardParserInterface {
    /**
     * @param document
     *         the information passed in for the OCR to be parsed
     * @return: Object containing contact information
     */
    ContactInfo getContactInfo(String document);

}
