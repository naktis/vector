#include <chrono> 

class Timer {
    private:
        using hrClock = std::chrono::high_resolution_clock;
        using durationDouble = std::chrono::duration<double>;
        std::chrono::time_point<hrClock> start;
    public:
        void set() {
            start= hrClock::now();
        }
        double elapsed() const{
            return durationDouble(hrClock::now() -start).count();
        }
};