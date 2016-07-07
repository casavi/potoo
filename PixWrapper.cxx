//
// Created by markus on 29/06/16.
//

#include "PixWrapper.hxx"

void pixDeleter(Pix *);

PixWrapper::PixWrapper(Pix *pix)
    : pix_ptr(pix, pixDeleter) {

}

Pix *PixWrapper::operator->() {
    return this->get();
}

Pix *PixWrapper::get() {
    return pix_ptr.get();
}

void pixDeleter(Pix *pix) {
    pixDestroy(&pix);
}
