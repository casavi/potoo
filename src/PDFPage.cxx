
#include "PDFPage.hxx"

#include "TesseractWrapper.hxx"

#include <cmath>
#include <mutex>

namespace gm = Magick;

thread_local std::unique_ptr<TesseractWrapper> _tesseractAPI;

PDFPage::PDFPage() {
}

void PDFPage::set_opts(const std::shared_ptr<Options> &opts) {
    _opts = opts;
    // Pre-reserve for all possible results for nice contiguous memory :)
    _results.reserve(_opts->_crops.size());
}

void PDFPage::put_page(PopplerPage &&page) {
    _page_number = page.page_number();
    _page.reset(new PopplerPage(std::move(page)));
    _page_rect = _page->size();
}

void PDFPage::process() {
    {
        namespace gm = Magick;

        if (!_tesseractAPI) {
            _tesseractAPI.reset(new TesseractWrapper(_opts->_language));
        }

        const float width_factor = static_cast<float>(_page_rect.width() / 100.f);
        const float height_factor = static_cast<float>(_page_rect.height() / 100.f);

        auto runner = [&](const decltype(_opts->_crops)::value_type &crop) {

            poppler::rectf r(std::floor(crop.x * width_factor),
                             std::floor(crop.y * height_factor),
                             std::ceil(crop.w * width_factor),
                             std::ceil(crop.h * height_factor)
            );

            auto result_utf8 = _page->text(r);


            std::string text;

            if (result_utf8.size()) {
                text.reserve(result_utf8.size());
                for (auto &c : result_utf8.to_utf8()) {
                    text += c;
                }
            }
            else {
                auto img = *image_representation();
                gm::Blob b;

                const size_t width = img.size().width();
                const size_t height = img.size().height();

                auto geo = gm::Geometry(
                    std::ceil(crop.w * width / 100.f),
                    std::ceil(crop.h * height / 100.f),
                    std::floor(crop.x * width / 100.f),
                    std::floor(crop.y * height / 100.f)
                );

                img.crop(
                    std::move(geo)
                );

                img.blur(0, 1);

                img.sharpen(3, 10);

                img.normalize();

                // Save it internally to avoid disk i/o
                // We write it to a binary blob here to read it via leptonicas pixReadMem
                img.write(&b, "png");

                // Read from memory, really fast
                auto pix = PixWrapper(pixReadMem(static_cast<l_uint8 *>(const_cast<void *>(b.data())), b.length()));

                (*_tesseractAPI)->SetImage(pix.get());

                // Wrap to enable auto-deletion of 'new'ly allocated text
                auto textptr = std::unique_ptr<char[]>((*_tesseractAPI)->GetUTF8Text());

                text = std::string(textptr.get());
            }

            _results.emplace_back(
                std::make_pair(
                    crop.type,
                    text
                )
            );
        };

        std::for_each(_opts->_crops.cbegin(), _opts->_crops.cend(), runner);

        _image.reset(nullptr);
        _page.reset(nullptr);
    }
}

const PDFPage::ResultList &PDFPage::get_results() const {
    return _results;
}

int PDFPage::get_page() const {
    return _page_number;
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

void PDFPage::set_text_hinting() {
    _page->set_text_hinting();
}
