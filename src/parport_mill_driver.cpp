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
    enum Flags {
        Calibrating = 0x1,
        Calibrated = 0x2,
        Bouncing = 0x4,   // set if we hit a limit switch and are reversing direction to move off the switch.
        FindLimits = 0x8
    };
    int  dir_bits;
    int  step_bits;
    int  bit_mask;
    int  limit_mask;  // bitmask for a limit switch (0 to disable)
    bool  flip_dir; // if we need to flip the direction bit (1 moves toward the motor)
    int max_steps; // the max number of steps away from the motor (determined by calibration)
    int flags;
    int micro_step;
    int motor_step;
    double pitch;
    int step_rev;
    double step_ratio;
    ParPortAxisController* controller;
    Timer* timer;
    ParPortMillDriverPrivate* priv;
    double ts;

    ParPortAxis(ParPortMillDriverPrivate* p, const char* n, Data& config)
            
            //int dirbits, int stepbits, int bitmask, int limitmask, bool fl, long max = 0, double ptch = 4.0l, int microstep = 8, int motorsteps = 200)
        : dir_bits(config["dir_bits"].asLong()),
          step_bits(config["step_bits"].asLong()),
          bit_mask(dir_bits | step_bits),
          limit_mask(config["limit_mask"].asLong()),
          flip_dir(true),
          max_steps(config["step_range"].asLong()),
          flags(0),
          micro_step(config["micro_step"].asLong()), 
          motor_step(config["motor_step"].asLong()),
          pitch(config["pitch"].asDouble()),
          step_rev(micro_step * motor_step),
          step_ratio(step_rev / pitch),
          controller(new ParPortAxisController(this)),
          timer(new Timer(controller)),
          priv(p),
          ts(0)
    {
        pos = 0;
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

    double distance(long steps)
    {
        return (steps >> 1) / step_ratio;
    }

    void zero() {
        this->pos = 0;
    }

    double position() {
        return this->pos / step_ratio;
    }

    double max_position() {
        if (this->max_steps) {
            return this->max_steps / step_ratio;
        }
        return -1;
    }

    void move_to(double p, double v, double a) {
        if (flags & Calibrated) {
            std::cout << axis << " axis: Moving absolute to " << p << "\n";

            Movement m;

            double d = p - position();
            m.direction = d < 0 ? 0 : 1;
            m.distance = fabs(d);
            m.velocity = v;
            m.acceleration = a;

            move(&m);
        } else {
            std::cout << axis << " axis: cant move absolute when not calibrated.\n";
        }
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

            // validate move against limits/position if we are calibrated
            if (flags & Calibrated) {
                long rem;
                if (move_state.direction) {
                    rem = (max_steps - pos) * 2;
                } else {
                    rem = (pos * 2);
                }

                if (rem < move_state.steps) {
                    std::cout << axis << " axis: Movement specified goes beyond axis limits, refusing\n"
                              << " asked " << move_state.steps << " steps/" << distance(move_state.steps) << "mm\n"
                              << " remaining " << rem << " steps/" << distance(rem) << "mm\n";
                        ;
                    move_state.steps = 0;
                }
            }

            if (move_state.steps) {
                ts = now();
                move_state.start_time = ts;
                timer->start(move_state.frequency);
                moving = 1;
            } else {
                moving = 0;
            }
        }
    }

    void calibrate();
    void step();
    void calibration_step();
}; 

void ParPortAxisController::tick()
{
    if (_axis->flags & ParPortAxis::Calibrating) {
        _axis->calibration_step();
    } else {
        _axis->step();
    }
}

//
//  7  6  5  4  3  2  1  0
// DA SA DZ SZ DY SY DX SX

class ParPortMillDriverPrivate
{
public:
    ParPortMillDriverPrivate(ParPort* p, Data& config)
        : port(p),
          x(this, "x", config["x"]),
          y(this, "y", config["y"]),
          z(this, "z", config["z"]),
          //x(this, "x", 0x82, 0x41, 0xc3, 0x40, true, 127694),
          //y(this, "y", 0x08, 0x04, 0x0c, 0x20, true, 127694),
          //z(this, "z", 0x20, 0x10, 0x30, 0x10, true, 60700),
          callback(0)
    {
    }

    void finished(ParPortAxis* a) {
        //printf("axis %s is done\n", a->axis);

        if (x.moving == 0 &&
            y.moving == 0 &&
            z.moving == 0) {
            //printf("movement complete\n");
            if (callback) {
                callback->movement_complete();
            }
        }
    }

    void limit_reached(ParPortAxis* a) {
        if (callback) {
            callback->limit_reached(a->axis);
        }

        finished(a);
    }

    ParPort* port;
    ParPortAxis x;
    ParPortAxis y;
    ParPortAxis z;
    double p[3];
    MillDriverCallback* callback;
};

void ParPortAxis::calibration_step()
{
    // check limit switches
    if (limit_mask) {
        int lim = priv->port->status();

        if (flags & Bouncing) {
            // expect limit bit to be set, so check for cleared, set zero.
            if ((lim & limit_mask) == 0) {
                limits = 0;
                flags &= ~Bouncing;
                // if we are currently moving away, then we are at the motor end (position 0)
                if (move_state.direction == 1) {
                    std::cout << axis << " found zero\n";
                    pos = 0;

                    // if we are finding limits, we are calibrated at this point and can move to the middle
                    if ((flags & FindLimits) == 0) {
                        flags |= Calibrated;
                        move_state.steps = max_steps;
                    }

                } else {
                    // otherwise we are away, moving toward the motor and are now done calibrating
                    std::cout << axis << " found max steps " << pos << "\n";
                    max_steps = pos;
                    flags |= Calibrated;

                    // move to the middle
                    move_state.steps = max_steps;
                }
            }
        } else {
            // we are at a limit switch
            if (lim & limit_mask) {
                if (move_state.direction == 0) {
                    if ((flags & Calibrated) == 0) {
                        limits = AtMin;
                        std::cout << axis << " at minimum\n";
                    }
                } else {
                    if (pos > 100) {
                        limits = AtMax;
                        std::cout << axis << " at max\n";
                    }
                }

                if (limits) {
                    flags |= Bouncing;
                    move_state.direction = 1 - move_state.direction;
                    move_state.steps = 1000;
                }
            }
        }
    }

    int d = move_state.direction ? dir_bits : 0;
    if (flip_dir) {
        d = move_state.direction ? 0 : dir_bits;
    }

    if ((move_state.steps & 0x1) == 0) {
        d |= step_bits;
    } else {
        pos = move_state.direction ? pos + 1 : pos - 1;
    }

    move_state.steps--;
    if (move_state.steps == 0) {
        if (flags & Calibrated) {
            flags = Calibrated;
            moving = 0;
            d = 0;
            timer->stop();
            priv->finished(this);
        } else {
            //std::cout << "c" << std::flush;
            move_state.steps = 1000;
        }
    }

    priv->port->merge(d, bit_mask);
}

void ParPortAxis::step()
{
    double t = now();

    int d = move_state.direction ? dir_bits : 0;
    if (flip_dir) {
        d = move_state.direction ? 0 : dir_bits;
    }

    if ((move_state.steps & 0x1) == 0) {
        d |= step_bits;
    } else {
        pos = move_state.direction ? pos + 1 : pos - 1;
    }

    priv->port->merge(d, bit_mask);

    // check limit switches
    if (limit_mask) {
        int lim = priv->port->status();

        // we are at a limit switch
        if (lim & limit_mask) {
            if (move_state.direction == 0) {
                limits = AtMin;
            } else {
                limits = AtMax;
            }

            moving = 0;
            timer->stop();
            priv->limit_reached(this);
        }
    }

    if (current_move.acceleration_function) {
        move_state.acceleration = current_move.acceleration_function(t - move_state.start_time);
    }

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
}

void ParPortAxis::calibrate()
{
    if (limit_mask) {
        int status = priv->port->status();
        if (status & limit_mask) {
            std::cout << axis << " axis: Can't calibrate as limit is reached. move away from limit switch\n";
            limits = AtMin | AtMax;
        } else {
            flags |= Calibrating;
            move_state.direction = 0;
            move_state.steps = 1000;
            move_state.velocity = 12;
            move_state.frequency = step_frequency(move_state.velocity);
            move_state.acceleration = 0;
            ts = now();
            move_state.start_time = ts;
            timer->start(move_state.frequency);
            moving = 1;
            std::cout << axis << " axis: beginning calibration\n" << move_state << "\n";
        }
    } else {
        std::cout << axis << " axis: Can't calibrate without limit switches\n";
    }
}

ParPortMillDriver::ParPortMillDriver(ParPort* p, Data& config)
    : _p(new ParPortMillDriverPrivate(p, config))
{
}

ParPortMillDriver::~ParPortMillDriver()
{
}

void ParPortMillDriver::move_absolute(double x, double y, double z, double vel, double acc, MillDriverCallback* cb)
{
    _p->x.move_to(x, vel, acc);
    _p->y.move_to(y, vel, acc);
    _p->z.move_to(z, vel, acc);
    _p->callback = cb;

    if (_p->x.moving == 0 &&
        _p->y.moving == 0 &&
        _p->z.moving == 0) {
        printf("no move\n");
        if (cb) {
            cb->movement_complete();
        }
    }
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

    //std::cout << "{" << _p->x << "\n" << _p->y << "\n" << _p->z << "}\n";
}

void ParPortMillDriver::zero(const char* axis)
{
    switch (axis[0]) {
        case 'x':
        case 'X':
            _p->x.pos = 0;
            break;
        case 'y':
        case 'Y':
            _p->y.pos = 0;
            break;
        case 'z':
        case 'Z':
            _p->z.pos = 0;
            break;
    }
}

void ParPortMillDriver::calibrate(MillDriverCallback* cb)
{
    _p->x.calibrate();
    _p->y.calibrate();
    _p->z.calibrate();
    _p->callback = cb;
}

double* ParPortMillDriver::position()
{
    _p->p[0] = _p->x.position();
    _p->p[1] = _p->y.position();
    _p->p[2] = _p->z.position();
    return _p->p;
}

double* ParPortMillDriver::max_position()
{
    _p->p[0] = _p->x.max_position();
    _p->p[1] = _p->y.max_position();
    _p->p[2] = _p->z.max_position();
    return _p->p;
}


