/* main.cpp */

#include <unistd.h>
#include <sched.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include <deque>

#include "timer.h"
#include "parport.h"
#include "parport_mill_driver.h"

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

static volatile int not_done = 1;

void sleep_till_done()
{
    while (not_done) {
        sleep(1);
    }
}

struct Move
{
    Movement x;
    Movement y;
    Movement z;
};

class MoveQueue : public MillDriverCallback
{
public:
    MoveQueue(MillDriver* d)
        : _d(d), _queue()
    {
        _x = 0;
        _y = 0;
        _z = 0;
        _vel = 12;
        _acc = 0;
    }

    ~MoveQueue() {}

    void movement_complete() {
        if (_queue.empty()) {
            not_done = 0;
        } else {
            Move& m = _queue.front();

            _d->move(&m.x, &m.y, &m.z, this);

            _queue.pop_front();
        }
    }

    void set(double x, double y, double z)
    {
        _x = x;
        _y = y;
        _z = z;
    }

    void move_to(double x, double y, double z)
    {
        double dx = _x - x;
        double dy = _y - y;
        double dz = _z - z;

        Move m;

        setup(m.x, _x, x);
        setup(m.y, _y, y);
        setup(m.z, _z, z, true);
        _x = x;
        _y = y;
        _z = z;

        _queue.push_back(m);
    }

    void go() {
        Move& m = _queue.front();

        _d->move(&m.x, &m.y, &m.z, this);

        _queue.pop_front();
    }

    void setup(Movement& m, double o, double n, bool f = false)
    {
        double d = o - n;
        m.direction = d < 0 ? 0 : 1;
        if (f) {
            m.direction = 1 - m.direction;
        }
        m.distance = fabs(d);
        m.velocity = _vel;
        m.acceleration = _acc;
    }

private:
    std::deque<Move> _queue;
    MillDriver*      _d;
    double _x, _y, _z;
    double _vel;
    double _acc;
};


int main(int argc, char** argv)
{
    if (geteuid() != 0) {
        printf("need to be root yo!\n");
        return 1;
    }

    // rt goodness
    struct sched_param parms;
    memset(&parms, 0, sizeof(parms));
    parms.sched_priority = 10;
    if (sched_setscheduler(getpid(), SCHED_FIFO, &parms) == -1) {
        perror("sched_setscheduler");
    }

    ParPortMillDriver driver("/dev/parport0");
    MoveQueue q(&driver);

    q.move_to(0, 0, 40);

    /*
    double z = -9;
    for (double y = 0; y < 50; y += 10) {
        for (double x = 0; x < 50; x += 10) {
            q.move_to(x, y, z);
            q.move_to(x+10, y, z);
            q.move_to(x+10, y+10, z);
            q.move_to(x, y+10, z);
            q.move_to(x, y, 0);
        }
        q.move_to(0, y, 0);
    }

    q.move_to(0, 0, 0);
    */

    //q.move_to(10, 0, 0);
    //q.move_to(10, 10, 0);
    //q.move_to(0, 10, 0);
    //q.move_to(0, 0, 0);

    q.go();

    sleep_till_done();

    /*
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
    */

    return 0;
}

