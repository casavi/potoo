//
// Created by markus on 29/06/16.
//

#ifndef CONVERTER_TIMER_HXX
#define CONVERTER_TIMER_HXX

#include <chrono>
#include <iostream>

/**
 * Crude timer class to enable easy benchmarking (text only for now)
 */
class Timer {
public:
    Timer(const std::string &name);

    ~Timer();

    void time();

    template<typename T>
    T duration(bool resolved = true){
        _timed = resolved;
        return std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - _start);
    }

private:
    bool _timed;
    std::string _name;
    std::chrono::high_resolution_clock::time_point _start;
};

#endif //CONVERTER_TIMER_HXX
