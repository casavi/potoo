
#include "PixWrapper.hxx"

void pixDeleter(Pix *);

PixWrapper::PixWrapper(Pix *pix)
    : _pix_ptr(pix, pixDeleter) {

}

PixWrapper::PixWrapper(PixWrapper &&other) : _pix_ptr(std::move(other._pix_ptr)) {

}

Pix *PixWrapper::operator->() {
    return this->get();
}

Pix *PixWrapper::get() {
    return _pix_ptr.get();
}

void pixDeleter(Pix *pix) {
    pixDestroy(&pix);
}
