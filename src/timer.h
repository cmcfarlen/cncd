/* timer.h */

#ifndef _GCODED_TIMER_H_
#define _GCODED_TIMER_H_

#include <functional>

double now();
double resolution();
void stay_asleep(double sec);

class TimerHandler
{
public:
    virtual ~TimerHandler() {}

    virtual void tick() = 0;
};

class LambdaHandler : public TimerHandler
{
public:
    LambdaHandler(std::function<void ()> f) : _f(f) {}

    void tick() {
        _f();
    }

private:
    std::function<void ()> _f;
};


class TimerPriv;
class Timer {
public:
    Timer(std::function<void ()> tick);
    Timer(TimerHandler* h);
    ~Timer();

    void create();
    void start(double f);
    void set_frequency(double f);
    void stop();
    void destroy();

private:
    TimerPriv* _p;
};


#endif

