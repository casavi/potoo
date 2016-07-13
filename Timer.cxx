
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
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::high_resolution_clock::now() - _start
              ).count()
              << std::endl;
    _timed = true;
}
