/* timer.cpp */

#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "timer.h"

double to_dtime(timespec& ts)
{
    return (double)ts.tv_sec + ts.tv_nsec / 1000000000.0l;
}

timespec to_tspec(double t)
{
    double s = floor(t);
    timespec ts;
    ts.tv_sec = s;
    ts.tv_nsec = (int)((t - s) * 1000000000);
    return ts;
}

double now()
{
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return to_dtime(ts);
}

itimerspec to_itspec(double f)
{
    itimerspec its;
    timespec ts(to_tspec(1.0l/f));

    its.it_value = ts;
    its.it_interval = ts;

    return its;
}

double resolution()
{
    static double res = 0;

    if (res == 0) {
        timespec  rts;
        clockid_t clk = CLOCK_MONOTONIC;

        clock_getres(clk, &rts);
        res = to_dtime(rts);
    }
    return res;
}

void stay_asleep(double sec)
{
    timespec ts(to_tspec(sec));
    timespec rem;
    int rc;

    do {
        rc = nanosleep(&ts, &rem);
        ts = rem;
    } while (rc == -1 && errno == EINTR);
    if (rc == -1) {
        perror("nanosleep");
    }
}

static int next_rtsig()
{
    static int sigcnt = 0;
    return SIGRTMIN + sigcnt++;
}

void timer_sighandler(int sig, siginfo_t* info, void*);

class TimerPriv {
public:
    timer_t sys_timer;
    int      sys_signal;
    TimerHandler* handler;
};

Timer::Timer(std::function<void ()> f)
    : _p(new TimerPriv())
{
    _p->handler = new LambdaHandler(f);
    create();
}

Timer::Timer(TimerHandler* h) : _p(new TimerPriv())
{
    _p->handler = h;
    create();
}

Timer::~Timer()
{
    destroy();
}

void Timer::create()
{
    clockid_t clk = CLOCK_MONOTONIC;

    struct sigaction sa;
    sigset_t  mask;
    sigevent sys_event;
    _p->sys_signal = next_rtsig();


    // setup action
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_sighandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(_p->sys_signal, &sa, NULL) == -1) {
        perror("sigaction");
    }
        
    // create a timer
    memset(&sys_event, 0, sizeof(sys_event));
    sys_event.sigev_notify = SIGEV_SIGNAL;
    sys_event.sigev_signo = _p->sys_signal;
    sys_event.sigev_value.sival_ptr = _p;
    if (timer_create(clk, &sys_event, &_p->sys_timer) == -1) {
        perror("timer_create");
    }
}

void Timer::start(double f) {
    itimerspec its(to_itspec(f));

    if (timer_settime(_p->sys_timer, 0, &its, NULL) == -1) {
        perror("timer_settime");
    }
}

void Timer::set_frequency(double f) {
    start(f);
}

void Timer::stop() {
    itimerspec its;
    memset(&its, 0, sizeof(its));
    if (timer_settime(_p->sys_timer, 0, &its, NULL) == -1) {
        perror("timer_settime");
    }
}

void Timer::destroy() { /* todo, maybe */ }

void timer_sighandler(int sig, siginfo_t* info, void*)
{
    TimerPriv* t = (TimerPriv*)(info->si_value.sival_ptr);
    t->handler->tick();
}

