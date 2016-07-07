//
// Created by markus on 29/06/16.
//

#ifndef CONVERTER_PIXWRAPPER_HXX
#define CONVERTER_PIXWRAPPER_HXX

#include <leptonica/allheaders.h>

#include "utils.hxx"

class PixWrapper {
public:
    PixWrapper(Pix* pix);

    Pix* operator->();

    Pix* get();
private:
    using PixPtr = deleted_unique_ptr<Pix>;

    PixPtr pix_ptr;
};


#endif //CONVERTER_PIXWRAPPER_HXX
