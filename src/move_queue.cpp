/* move_queue.cpp */

#include <math.h>
#include "move_queue.h"

MoveQueue::MoveQueue(MillDriver* d)
: _d(d), _queue(), _update_timer(this)
{
    _x = 0;
    _y = 0;
    _z = 0;
    _sx = -1;
    _sy = -1;
    _sz = -1;
    _vel = 12;
    _fvel = 6;
    _acc = 0;
    _done = 0;
}

MoveQueue::~MoveQueue() {}

void MoveQueue::movement_complete() {

    if (_pcb) {
        _pcb(_d->position());
    }

    if (_queue.empty()) {
        _update_timer.stop();
        _done = 1;
        if (_cb) {
            _cb();
        }
    } else {
        Move m = _queue.front();
        _queue.pop_front();

        if (m.zero) {
            std::cout << "zeroing axis.";

            _x = 0;
            _y = 0;
            _z = 0;

            movement_complete();
            return;
        }

        if (m.calibrate) {
            _d->calibrate(this);
        } else {
            _d->move(&m.x, &m.y, &m.z, this);
        }

    }
}

void MoveQueue::limit_reached(char* axis) {
    std::cout << "limit reached for axis: " << axis[0] << "\n";

    _d->zero(axis);
}

void MoveQueue::tick()
{
    if (_pcb) {
        _pcb(_d->position());
    }
}

void MoveQueue::zero()
{
    _x = 0;
    _y = 0;
    _z = 0;

    std::cout << "ZERO\n";
}

void MoveQueue::calibrate()
{
    Move m;
    m.calibrate = 1;
    _queue.push_back(m);

    std::cout << "CALIBRATE\n";
}

void MoveQueue::set(double x, double y, double z)
{
    _x = x;
    _y = y;
    _z = z;
}

void MoveQueue::move(double dx, double dy, double dz)
{
    move_to(_x+dx, _y+dy, _z+dz);
}

void MoveQueue::move_to(double x, double y, double z)
{
    double dx = _x - x;
    double dy = _y - y;
    double dz = _z - z;

    double dist = sqrt(dx*dx + dy*dy + dz*dz);

    Move m;

    setup(m.x, _x, x, dist);
    setup(m.y, _y, y, dist);
    setup(m.z, _z, z, dist);
    _x = x;
    _y = y;
    _z = z;

    _queue.push_back(m);

    std::cout << "RAPID: " << m << "\n";
}

void MoveQueue::feed(double dx, double dy, double dz)
{
    feed_to(_x+dx, _y+dy, _z+dz);
}

void MoveQueue::feed_to(double x, double y, double z)
{
    double dx = _x - x;
    double dy = _y - y;
    double dz = _z - z;

    double dist = sqrt(dx*dx + dy*dy + dz*dz);
    Move m;

    setup_feed(m.x, _x, x, dist);
    setup_feed(m.y, _y, y, dist);
    setup_feed(m.z, _z, z, dist);
    _x = x;
    _y = y;
    _z = z;

    _queue.push_back(m);
    std::cout << "FEED: " << m << "\n";
}

void MoveQueue::go(std::function<void ()> func) {
    _cb = func;

    if (_pcb) {
        _pcb(_d->position());
        _update_timer.start(30);
    }

    // go to start position
    if (_sx != -1) {
        double* p = _d->position();
        Move m;

        std::cout << "Moving to start position: " << _sx << " " << _sy << " " << _sz 
                  << "\nfrom " << p[0] << " " << p[1] << " " << p[2] << "\n";

        _d->move_absolute(_sx, _sy, _sz, _vel, 0, this);
    } else {
        // start on first movement otherwise
        movement_complete();
    }
}

void MoveQueue::setup(Movement& m, double o, double n, double dist)
{
    double d = n - o;
    m.direction = d < 0 ? 0 : 1;
    m.distance = fabs(d);
    m.velocity = (m.distance / dist) * _vel;
    m.acceleration = _acc;
}

void MoveQueue::setup_feed(Movement& m, double o, double n, double dist)
{
    double d = o - n;
    m.direction = d < 0 ? 0 : 1;
    m.distance = fabs(d);
    m.velocity = (d / dist) * _fvel;
    m.acceleration = 0;
}




