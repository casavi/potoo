//
// Created by markus on 30/06/16.
//

#ifndef CONVERTER_PDF_HXX
#define CONVERTER_PDF_HXX

#include "Options.hxx"
#include "Poppler.hxx"
#include <boost/property_tree/ptree.hpp>
#include <Magick++/Image.h>
#include <mutex>

/**
 * Toplevel PDF worker class.
 * Handles everything regarding the PDF itself, like extracting pages.
 */
class PDF {

    // Aliased for easier usage
    using ResultList = std::vector<std::pair<std::string, std::string>>;

    struct PDFPage {

        PDFPage(PopplerPage &&_page);

        /**
         * Performs OCR or text extraction on one page and returns the results
         */
        ResultList process(const std::string &language, const std::vector<Options::Crop> &crops);

        Magick::Image image_representation(int dpi);

        PopplerPage _page;
    };

public:

    PDF(const std::shared_ptr<Options> &options);

    /**
     * The main function, this does all the work and coordinates everything.
     * Launches multiple working threads (via tbb).
     * @return A ptree containing the results.
     */
    boost::property_tree::ptree work();

    void write_page(int page_number, const std::string &path);

    size_t page_count() const;

    boost::property_tree::ptree results_as_ptree() const;

private:
    std::vector<int> _pdf_page_numbers;
    std::vector<std::pair<int, ResultList>> _results;
    const std::shared_ptr<Options> _opts;
    PopplerDocument _document;
    std::mutex _mutex;
};


#endif //CONVERTER_PDF_HXX
