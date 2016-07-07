//
// Created by markus on 29/06/16.
//

#include "Timer.hxx"

Timer::Timer(const std::string &name)
    :
    name(name),
    start(std::chrono::high_resolution_clock::now()),
    timed(false) {

}

Timer::~Timer() {
    if (!timed)
        time();
}

void Timer::time() {
    std::cout << name << ": "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::high_resolution_clock::now() - start
              ).count()
              << std::endl;
    timed = true;
}
