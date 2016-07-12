
#include "Poppler.hxx"

using namespace poppler;

PopplerPage::PopplerPage(page *page) : _page(page), _renderer(std::unique_ptr<page_renderer>(new page_renderer())) {}

image PopplerPage::render(int dpi) const {
    return image(_renderer->render_page(_page.get(), dpi, dpi));
}

PopplerPage::PopplerPage(PopplerPage &&other) : _page(std::move(other._page)), _renderer(std::move(other._renderer)) {}

PopplerDocument::PopplerDocument(const std::string &input_pdf) {
    _document.reset(document::load_from_file(input_pdf));
    if (!_document) {
        throw std::runtime_error("Could not open " + input_pdf);
    } else if (_document->is_locked()) {
        throw std::runtime_error("File is locked " + input_pdf);
    }
}

size_t PopplerDocument::page_count() {
    return static_cast<size_t>(_document->pages());
}

PopplerPage PopplerDocument::get_page(int page_number) {
    return PopplerPage(_document->create_page(page_number));
}

PopplerDocument::PopplerDocument(PopplerDocument &&other) : _document(std::move(other._document)) {}
