//
// Created by markus on 30/06/16.
//

#ifndef CONVERTER_PDFPAGE_HXX
#define CONVERTER_PDFPAGE_HXX

#include "Options.hxx"
#include "PixWrapper.hxx"
#include "Poppler.hxx"

#include <Magick++.h>

#include <memory>
#include <vector>

class PDFPage {
public:
    // Aliased for easier usage
    using ResultList = std::vector<std::pair<std::string, std::string>>;

    PDFPage();

    /**
     * Dependency-inject the program options.
     * @param opts
     */
    void set_opts(const std::shared_ptr<Options> &opts);

    void set_page_number(int page);

    /**
     * PDF puts one extracted page here. Converts the PopplerPage to a Magick++ Image.
     * @param page
     */
    void put_page(const PopplerPage &page);

    /**
     * Does everything one page can do for every region on the page:
     *  - crop it
     *  - blur
     *  - sharpen
     *  - normalize
     *  - send to tesseract api
     *  - OCR it
     *  - fills ResultList
     */
    void process();

    const Magick::Image &get_image_representation();

    const ResultList &get_results() const;

    int get_page() const;

private:
    ResultList _results;
    Magick::Image _image;
    std::shared_ptr<Options> _opts;
    int _page;
};


#endif //CONVERTER_PDFPAGE_HXX
