//
// Created by markus on 29/06/16.
//

#ifndef CONVERTER_PIXWRAPPER_HXX
#define CONVERTER_PIXWRAPPER_HXX

#include <leptonica/allheaders.h>
#include <boost/core/noncopyable.hpp>

#include "utils.hxx"

/**
 * Simple wrapper class for leptonica Pix
 */
class PixWrapper : boost::noncopyable{
public:
    PixWrapper(Pix *pix);

    // We have to define it ourselves here to enable && construction
    PixWrapper(PixWrapper&& other);

    Pix *operator->();

    Pix *get();

private:
    using PixPtr = deleted_unique_ptr<Pix>;

    PixPtr _pix_ptr;
};


#endif //CONVERTER_PIXWRAPPER_HXX
