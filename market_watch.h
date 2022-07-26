//
// Created by Cephas Svosve on 20/12/2021.
//

#ifndef MEH_TIME_POINT_H
#define MEH_TIME_POINT_H
#include <chrono>


using namespace std::chrono;

class market_watch{

private:

    time_point<std::chrono::steady_clock, std::chrono::duration<long long int, std::ratio<1, 1000000000>>>
            start_time,
            creation_time;
    double manual_time = 0;
    double terminal_time = 0;



public:
    enum dt {one_microsecond = 0, one_millisecond=1, one_second=2, one_minute=3, manual = 4};
    dt day_length;


    explicit market_watch(dt day_length):creation_time(std::chrono::high_resolution_clock::now()){
        this->day_length = day_length;
    }

    void start();

    double
    current_time();

    void
    tick();

    bool
    ticking();

    time_point<std::chrono::steady_clock, std::chrono::duration<long long int, std::ratio<1, 1000000000>>>
    get_creation_time();

    void reset();
    void set_terminal_time(double tm);
    double get_terminal_time() const;
};


#endif //MEH_TIME_POINT_H
