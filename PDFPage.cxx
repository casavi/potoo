
#include "PDFPage.hxx"

#include "TesseractWrapper.hxx"

#include <cmath>
#include <iostream>
#include <mutex>

namespace gm = Magick;

PDFPage::PDFPage() {
}

void PDFPage::set_opts(const std::shared_ptr<Options> &opts) {
    _opts = opts;
    // Pre-reserve for all possible results for nice contiguous memory :)
    _results.reserve(_opts->_crops.size());
}

void PDFPage::put_page(PopplerPage &&page) {
    _page.reset(new PopplerPage(std::move(page)));
}

void PDFPage::process() {
    namespace gm = Magick;
    TesseractWrapper api(_opts->_language);

    auto runner = [&](const decltype(_opts->_crops)::value_type &crop) {

        auto realrect = _page->size();

        poppler::rectf r(std::floor(crop.x * realrect.width() / 100.f),
                         std::floor(crop.y * realrect.height() / 100.f),
                         std::ceil(crop.w * realrect.width() / 100.f),
                         std::ceil(crop.h * realrect.height() / 100.f)
        );

        auto result_utf8 = _page->text(r);
        std::string text;

        if (result_utf8.size() && false) {
            text.reserve(result_utf8.size());
            for(auto& c : result_utf8.to_utf8()){
                text += c;
            }
        }
        else {
            gm::Image img(*image_representation());
            gm::Blob b;

            // Calculate our percentages to fit the picture regardless of the real resolution.
            const size_t width = img.size().width();
            const size_t height = img.size().height();

            img.crop(
                gm::Geometry(
                    std::ceil(crop.w * width / 100.f),
                    std::ceil(crop.h * height / 100.f),
                    std::floor(crop.x * width / 100.f),
                    std::floor(crop.y * height / 100.f)
                )
            );
            img.blur(0, 1);
            img.sharpen(3, 10);
            img.normalize();

            // Save it internally to avoid disk i/o
            // We write it to a binary blob here to read it via leptonicas pixReadMem
            img.write(&b, "png");

            // Read from memory, really fast
            auto pix = PixWrapper(pixReadMem(static_cast<l_uint8 *>(const_cast<void *>(b.data())), b.length()));
            api->SetImage(pix.get());

            // Wrap to enable auto-deletion of 'new'ly allocated text
            auto textptr = std::unique_ptr<char[]>(api->GetUTF8Text());
            text = std::string(textptr.get());
        }

        _results.emplace_back(
            std::make_pair(
                crop.type,
                text
            )
        );
    };

    // Here we can enable parallel cropping if needed, may come in the future. Needs a lot of regions to even make sense
    // but even then I don't think it can improve anything if the pdf has enough pages
    //if(_opts->_crops.size() > xx){
    //    tbb::parallel_for_each(_opts->_crops.cbegin(), _opts->_crops.cend(), runner);
    //} else
    std::for_each(_opts->_crops.cbegin(), _opts->_crops.cend(), runner);
    //}

}

const PDFPage::ResultList &PDFPage::get_results() const {
    return _results;
}

int PDFPage::get_page() const {
    return _page->page_number();
}

const std::unique_ptr<Magick::Image> &PDFPage::image_representation() {
    if (!_image) {
        const auto img = _page->render(_opts->_dpi);

        _image.reset(new gm::Image());
        _image->quiet(false);

        // poppler::_image format differs from argb32 - this is unexpected and should never happen
        // if it does happen, poppler most likely changed something internal - try an older version
        // we have to be sure this format is used to ensure compatibility to Magick++
        assert(img.format() == poppler::image::format_enum::format_argb32);

        // Here we convert a poppler::_image to a Magick::Image
        // It looks very hacky but should work all the time, as long as poppler doesn't change their internal
        // representation of 4 B R G A bytes (chars) per pixel
        _image->read(
            static_cast<const unsigned int>(img.width()),
            static_cast<const unsigned int>(img.height()),
            "BGRA",
            gm::StorageType::CharPixel,
            img.const_data()
        );
    }
    return _image;
}
