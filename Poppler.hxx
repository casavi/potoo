//
// Created by markus on 30/06/16.
//

#ifndef CONVERTER_POPPLER_HXX
#define CONVERTER_POPPLER_HXX

#include "utils.hxx"

#include <memory>

#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-document.h>

#include <boost/core/noncopyable.hpp>

/**
 * Simple wrapper calss for poppler::page
 */
struct PopplerPage : public boost::noncopyable {
public:
    PopplerPage(poppler::page *page);

    // Lok at PixWrapper(PixWrapper&&)
    PopplerPage(PopplerPage &&other);

    poppler::image render(int dpi) const;

private:
    std::unique_ptr<poppler::page> _page;

    // Have to use a unique_ptr here to enable move construction
    std::unique_ptr<poppler::page_renderer> _renderer;
};

/**
 * Simple wrapper calss for poppler::document
 */
struct PopplerDocument {
public:
    PopplerDocument(const std::string &input_pdf);

    // Lok at PixWrapper(PixWrapper&&)
    PopplerDocument(PopplerDocument &&other);

    size_t page_count();

    PopplerPage get_page(int page_number);

private:
    std::unique_ptr<poppler::document> _document;
};


#endif //CONVERTER_POPPLER_HXX
