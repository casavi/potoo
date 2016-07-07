//
// Created by markus on 29/06/16.
//

#ifndef CONVERTER_UTILS_HXX
#define CONVERTER_UTILS_HXX

#include <memory>

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T *)>>;

namespace std {
    template<typename T, typename ...Args>
    std::unique_ptr<T> make_unique(Args &&...args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}


#endif //CONVERTER_UTILS_HXX
