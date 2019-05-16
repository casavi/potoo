#include <GlobalParams.h>
#include <Error.h>
#include "Poppler.hxx"

using namespace poppler;

PopplerPage::PopplerPage(poppler::page *page, int _page_number)
    : _page(page), _renderer(std::unique_ptr<page_renderer>(new page_renderer())), _page_number(_page_number) {}

image PopplerPage::render(int dpi) const {
    return image(_renderer->render_page(_page.get(), dpi, dpi));
}

PopplerPage::PopplerPage(PopplerPage &&other)
    : _page(std::move(other._page)), _renderer(std::move(other._renderer)), _page_number(other._page_number) {}

rectf PopplerPage::size() const {
    return _page->page_rect(poppler::page_box_enum::crop_box);
}

poppler::ustring PopplerPage::text(const poppler::rectf &rect) const {
    return _page->text(rect);
}

int PopplerPage::page_number() const {
    return _page_number;
}

void PopplerPage::set_text_hinting() {
    _renderer->set_render_hint(poppler::page_renderer::render_hint::text_antialiasing, true);
}

PopplerDocument::PopplerDocument(const std::string &input_pdf) {
    _document.reset(document::load_from_file(input_pdf));
    if (!_document) {
        throw std::runtime_error("Could not open " + input_pdf);
    } else if (_document->is_locked()) {
        throw std::runtime_error("File is locked " + input_pdf);
    }

    // Turn off error messages (no error sent to console).
    globalParams->setErrQuiet(true);
    setErrorCallback(nullptr, nullptr);
}

size_t PopplerDocument::page_count() const {
    return static_cast<size_t>(_document->pages());
}

PopplerPage PopplerDocument::get_page(int page_number) const {
    if(page_number >= _document->pages()){
        throw std::runtime_error("this page does not exist in this document");
    }
    return PopplerPage(_document->create_page(page_number), page_number);
}

PopplerDocument::PopplerDocument(PopplerDocument &&other) : _document(std::move(other._document)) {}
