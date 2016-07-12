
#include "PDFPage.hxx"

#include "TesseractWrapper.hxx"

#include <cmath>
#include <cassert>

namespace gm = Magick;

PDFPage::PDFPage() {
    _image.quiet(false);
}

void PDFPage::set_opts(const std::shared_ptr<Options> &opts) {
    _opts = opts;
    // Pre-reserve for all possible results for nice contiguous memory :)
    _results.reserve(_opts->_crops.size());
}

void PDFPage::set_page_number(int page) {
    _page = page;
}

void PDFPage::put_page(const PopplerPage &page) {
    const auto img = page.render(_opts->_dpi);

    // poppler::_image format differs from argb32 - this is unexpected and should never happen
    // if it does happen, poppler most likely changed something internal - try an older version
    // we have to be sure this format is used to ensure compatibility to Magick++
    assert(img.format() == poppler::image::format_enum::format_argb32);

    // Here we convert a poppler::_image to a Magick::Image
    // It looks very hacky but should work all the time, as long as poppler doesn't change their internal
    // representation of 4 B R G A bytes (chars) per pixel
    _image.read(
        static_cast<const unsigned int>(img.width()),
        static_cast<const unsigned int>(img.height()),
        "BGRA",
        gm::StorageType::CharPixel,
        img.const_data()
    );
}

void PDFPage::process() {
    namespace gm = Magick;

    // Calculate our percentages to fit the picture regardless of the real resolution.
    const size_t width = _image.size().width();
    const size_t height = _image.size().height();
    const float wf = width / 100.f;
    const float hf = height / 100.f;

    // convert to grayscale - works much better
    _image.type(gm::GrayscaleType);

    auto runner = [&](const decltype(_opts->_crops)::value_type &crop) {
        // This doesn't copy, it just references the original image
        gm::Image img(_image);
        gm::Blob b;
        TesseractWrapper api(_opts->_language);

        // Cropping and applying all corrections and improvements
        img.crop(
            gm::Geometry(
                std::floor(crop.w * wf),
                std::floor(crop.h * hf),
                std::floor(crop.x * wf),
                std::floor(crop.y * hf)
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
        auto text = std::unique_ptr<char[]>(api->GetUTF8Text());

        _results.emplace_back(
            std::make_pair(
                crop.type,
                std::string(text.get())
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
    return _page;
}

const Magick::Image &PDFPage::get_image_representation() {
    return _image;
}
