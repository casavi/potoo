//
// Created by markus on 29/06/16.
//

#ifndef CONVERTER_UTILS_HXX
#define CONVERTER_UTILS_HXX

#include <memory>
#include <functional>

// May get deleted in the future, depending on future needs.

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T *)>>;


#endif //CONVERTER_UTILS_HXX
