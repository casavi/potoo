
#include "TesseractWrapper.hxx"
#include <stdexcept>

void tesseractAPIDeleter(tesseract::TessBaseAPI *);

TesseractWrapper::TesseractWrapper(const std::string &language)
    : tesseract_api(new tesseract::TessBaseAPI(), tesseractAPIDeleter) {
    if (tesseract_api->Init(NULL, language.c_str())) {
        throw std::runtime_error("Could not initialize tesseract");
    }
}

tesseract::TessBaseAPI *TesseractWrapper::operator->() {
    return tesseract_api.get();
}

void tesseractAPIDeleter(tesseract::TessBaseAPI *api) {
    api->End();
    delete api;
}
