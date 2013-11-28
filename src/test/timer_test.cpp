/* timer_test.cpp */

#include <stdio.h>

#include "timer.h"

class TestHandler : public TimerHandler
{
public:
    double start_time;
    int count;

    TestHandler() : start_time(now())
    {}

    void tick()
    {
        count++;
    }

    void reset() {
        start_time = now();
        count = 0;
    }

    void report() {
        double end_time = now();

        printf("ticked %i times in %g seconds\n", count, end_time - start_time);
    }
};

int main(int argc, char** argv)
{
    TestHandler h;
    TestHandler h2;
    TestHandler h3;
    Timer t(&h);
    Timer t2(&h2);
    Timer t3(&h3);

    h.reset();
    h2.reset();
    h3.reset();
    t.start(10000.0);
    t2.start(10000.0);
    t3.start(10000.0);

    stay_asleep(4);
    h.report();
    h2.report();
    h3.report();
}
