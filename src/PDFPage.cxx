#include "PDF.hxx"
#include "TesseractWrapper.hxx"
#include "PixWrapper.hxx"

#include <cmath>

thread_local std::unique_ptr<TesseractWrapper> _tesseractAPI;

namespace gm = Magick;

PDF::PDFPage::PDFPage(PopplerPage &&page) :
    _page(std::move(page)) {
}

PDF::ResultList PDF::PDFPage::process(const std::string &language, const std::vector<Options::Crop> &crops, int dpi)
{
    namespace gm = Magick;

    ResultList results;

    poppler::rectf page_rect;
    page_rect = _page.size();

    if (!_tesseractAPI) {
        _tesseractAPI.reset(new TesseractWrapper(language));
    }

    const float width_factor = static_cast<float>(page_rect.width() / 100.f);
    const float height_factor = static_cast<float>(page_rect.height() / 100.f);

    auto runner = [&](const Options::Crop &crop) {

        poppler::rectf r(std::floor(crop.x * width_factor),
                         std::floor(crop.y * height_factor),
                         std::ceil(crop.w * width_factor),
                         std::ceil(crop.h * height_factor)
        );

        auto result_utf8 = _page.text(r);

        std::string text;

        if (result_utf8.size()) {
            text.reserve(result_utf8.size());
            for (auto &c : result_utf8.to_utf8()) {
                text += c;
            }
        }
        else {
            auto img = image_representation(dpi);
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

        results.emplace_back(
            std::make_pair(
                crop.type,
                text
            )
        );
    };

    std::for_each(crops.cbegin(), crops.cend(), runner);

    return results;
}

Magick::Image PDF::PDFPage::image_representation(int dpi) {
    const auto img = _page.render(dpi);

    gm::Image image;
    image.quiet(false);

    // poppler::_image format differs from argb32 - this is unexpected and should never happen
    // if it does happen, poppler most likely changed something internal - try an older version
    // we have to be sure this format is used to ensure compatibility to Magick++
    assert(img.format() == poppler::image::format_enum::format_argb32);

    // Here we convert a poppler::_image to a Magick::Image
    // It looks very hacky but should work all the time, as long as poppler doesn't change their internal
    // representation of 4 B R G A bytes (chars) per pixel
    image.read(
        static_cast<const unsigned int>(img.width()),
        static_cast<const unsigned int>(img.height()),
        "BGRA",
        gm::StorageType::CharPixel,
        img.const_data()
    );
    return image;
}
