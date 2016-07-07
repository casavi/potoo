//
// Created by markus on 30/06/16.
//

#ifndef CONVERTER_PDF_HXX
#define CONVERTER_PDF_HXX

#include "PDFPage.hxx"

#include "Options.hxx"
#include <boost/property_tree/ptree.hpp>

/**
 * Toplevel PDF worker class.
 * Handles everything regarding the PDF itself, like extracting pages.
 */
class PDF {
public:

    PDF(const std::shared_ptr<Options> &options);

    /**
     * The main function, this does all the work and coordinates everything.
     * Launches multiple working threads (via tbb).
     * @return A ptree containing the results.
     */
    boost::property_tree::ptree work();

    /**
     * Get one rendered PDFPage from here, mainly for the first_page parameter.
     * @param page_number 0-based page index.
     * @return The rendered PDFPage.
     */
    PDFPage get_page(int page_number);

private:
    std::vector<PDFPage> _pdf_pages;
    const std::shared_ptr<Options> _opts;
    PopplerDocument _document;
};


#endif //CONVERTER_PDF_HXX
