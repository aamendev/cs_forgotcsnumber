#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <types.h>
namespace lina { namespace core { 
    class timer
    {
        public:
            inline void begin() {mstart = std::chrono::steady_clock::now();}
            inline void end() {mend = std::chrono::steady_clock::now();};
            inline f32 get_time(timer_unit tu = timer_unit::ms) 
            {return ((std::chrono::duration<f32>(mend-mstart)).count()) * (float)(tu);};
        private:
            std::chrono::time_point<std::chrono::steady_clock> mstart;
            std::chrono::time_point<std::chrono::steady_clock> mend;

    };
}}
#endif
