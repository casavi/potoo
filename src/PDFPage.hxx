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
     * @param opts Options
     */
    void set_opts(const std::shared_ptr<Options> &opts);

    /**
     * PDF puts one extracted page here. Converts the PopplerPage to a Magick++ Image.
     * @param page PopplerPage rvalue reference
     */
    void put_page(PopplerPage &&page);

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

    const std::unique_ptr<Magick::Image>& image_representation();

    const ResultList &get_results() const;

    int get_page() const;

private:
    ResultList _results;
    int _page_number;
    std::shared_ptr<Options> _opts;
    std::unique_ptr<PopplerPage> _page;
    std::unique_ptr<Magick::Image> _image;
    poppler::rectf _page_rect;
};


#endif //CONVERTER_PDFPAGE_HXX
