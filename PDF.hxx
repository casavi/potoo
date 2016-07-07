//
// Created by markus on 30/06/16.
//

#ifndef CONVERTER_PDF_HXX
#define CONVERTER_PDF_HXX

#include "PDFPage.hxx"

#include "Options.hxx"
#include <boost/property_tree/ptree.hpp>


class PDF {
public:

    PDF(const std::shared_ptr<Options> &options);

    boost::property_tree::ptree work();

    PDFPage get_page(int page_number);

private:
    std::vector<PDFPage> _pdf_pages;
    const std::shared_ptr<Options> _opts;
    PopplerDocument _document;
};


#endif //CONVERTER_PDF_HXX
