/* parport_mill_driver.cpp */

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "parport_mill_driver.h"
#include "timer.h"

struct ParPortAxis;
class ParPortMillDriverPrivate;

class ParPortAxisController : public TimerHandler
{
public:
    ParPortAxisController(ParPortAxis* axis) : _axis(axis)
    {}
    virtual ~ParPortAxisController()
    {}

    void tick();

private:
    ParPortAxis* _axis;
};

struct ParPortAxis : public Axis
{
    int  shift;  // bits over for this axis (x=0 y=2 z=4)
    long step_position;
    int micro_step;
    int step_rev;
    double pitch;
    double step_ratio;
    ParPortAxisController* controller;
    Timer* timer;
    ParPortMillDriverPrivate* priv;
    double ts;

    ParPortAxis(ParPortMillDriverPrivate* p, const char* n, int bitshift, double ptch = 4.0l, int microstep = 8, int motorsteps = 200)
        : shift(bitshift),
          micro_step(microstep), 
          pitch(ptch),
          step_rev(micro_step * motorsteps),
          step_ratio(step_rev / pitch),
          controller(new ParPortAxisController(this)),
          timer(new Timer(controller)),
          priv(p)
    {
        moving = 0;
        strncpy(axis, n, 2);
    }

    double step_frequency(double v)
    {
        return 2.0l * step_ratio * v;
    }

    long steps(double dist)
    {
        double revs = dist / pitch;
        return 2 * (int)floor(step_rev * revs);
    }

    void move(Movement* m)
    {
        if (m && m->distance) {
            current_move = *m;
            move_state.steps = steps(m->distance);
            if (m->acceleration > 0) {
                move_state.velocity = 1;
                move_state.acceleration = m->acceleration;
            } else {
                move_state.velocity = m->velocity;
                move_state.acceleration = 0;
            }
            move_state.direction = m->direction ? 1 : 0;
            move_state.frequency = step_frequency(move_state.velocity);
            ts = now();
            timer->start(move_state.frequency);
            moving = 1;
        }
    }

    void step();
}; 

void ParPortAxisController::tick()
{
    _axis->step();
}

class ParPortMillDriverPrivate
{
public:
    ParPortMillDriverPrivate(const std::string& path)
        : port(path),
          x(this, "x", 0),
          y(this, "y", 2),
          z(this, "z", 4),
          callback(0)
    {
    }

    void finished(ParPortAxis* a) {
        printf("axis %s is done\n", a->axis);

        if (x.moving == 0 &&
            y.moving == 0 &&
            z.moving == 0) {
            printf("movement complete\n");
            if (callback) {
                callback->movement_complete();
            }
        }
    }

    ParPort port;
    ParPortAxis x;
    ParPortAxis y;
    ParPortAxis z;
    MillDriverCallback* callback;
};


void ParPortAxis::step()
{
    double t = now();
    int d = move_state.direction << 1;
    if (move_state.steps & 0x1) {
        d |= 1;
    }
    priv->port.merge(d << shift, 0x3 << shift);

    if (move_state.acceleration > 0) {
        move_state.velocity += (t - ts) * move_state.acceleration;
        move_state.frequency = step_frequency(move_state.velocity);
        timer->set_frequency(move_state.frequency);
        if (move_state.velocity >= current_move.velocity) {
            move_state.acceleration = 0;
        }
    }

    move_state.steps--;
    ts = t;
    if (move_state.steps == 0) {
        moving = 0;
        timer->stop();
        priv->finished(this);
    }

    //std::cout << *this << " " << std::hex << priv->port.data() << std::dec << "\n";
}

ParPortMillDriver::ParPortMillDriver(const std::string& path)
    : _p(new ParPortMillDriverPrivate(path))
{
}

ParPortMillDriver::~ParPortMillDriver()
{
}

void ParPortMillDriver::move(Movement* x, Movement* y, Movement* z, MillDriverCallback* cb)
{
    _p->x.move(x);
    _p->y.move(y);
    _p->z.move(z);
    _p->callback = cb;

    if (_p->x.moving == 0 &&
        _p->y.moving == 0 &&
        _p->z.moving == 0) {
        printf("no move\n");
        if (cb) {
            cb->movement_complete();
        }
    }

    std::cout << "{" << _p->x << "\n" << _p->y << "\n" << _p->z << "}\n";
}


