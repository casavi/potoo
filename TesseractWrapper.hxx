//
// Created by markus on 29/06/16.
//

#ifndef CONVERTER_TESSERACTWRAPPER_HXX
#define CONVERTER_TESSERACTWRAPPER_HXX

#include <tesseract/baseapi.h>

#include "utils.hxx"

class TesseractWrapper {

public:
    TesseractWrapper(const std::string& language);

    tesseract::TessBaseAPI* operator->();

private:
    using TesseractAPIPtr = deleted_unique_ptr<tesseract::TessBaseAPI>;

    TesseractAPIPtr tesseract_api;
};


#endif //CONVERTER_TESSERACTWRAPPER_HXX
