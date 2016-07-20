
#include "Timer.hxx"

Timer::Timer(const std::string &name)
    :
    _name(name),
    _start(std::chrono::high_resolution_clock::now()),
    _timed(false) {

}

Timer::~Timer() {
    if (!_timed)
        time();
}

void Timer::time() {
    std::cout << _name << ": "
              << duration<std::chrono::milliseconds>().count()
              << std::endl;
}
