#include "timer_tests.h"

namespace lina { namespace tests {
    void sleep_test()
    {
        core::timer t; 
        using namespace std::chrono_literals;

        BEGIN_TEST("100ms sleep");
        t.begin();
        std::this_thread::sleep_for(100ms);
        t.end();
        assert(std::abs(t.get_time() - 100) < 1);
        std::cerr << t.get_time() << '\n';
        END_TEST("100ms sleep");
        BEGIN_TEST("1s sleep");
        t.begin();
        std::this_thread::sleep_for(1000ms);
        t.end();
        assert(std::abs(t.get_time() - 1000) < 1);
        END_TEST("1sec sleep");
    };
}}
