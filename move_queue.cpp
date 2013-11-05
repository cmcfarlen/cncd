/* move_queue.cpp */

#include <math.h>
#include "move_queue.h"

MoveQueue::MoveQueue(MillDriver* d)
: _d(d), _queue()
{
    _x = 0;
    _y = 0;
    _z = 0;
    _vel = 12;
    _fvel = 6;
    _acc = 0;
    _done = 0;
}

MoveQueue::~MoveQueue() {}

void MoveQueue::movement_complete() {
    if (_queue.empty()) {
        if (_cb) {
            _cb();
        }
        _done = 1;
    } else {
        Move& m = _queue.front();

        _d->move(&m.x, &m.y, &m.z, this);

        _queue.pop_front();
    }
}

void MoveQueue::set(double x, double y, double z)
{
    _x = x;
    _y = y;
    _z = z;
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
    setup(m.z, _z, z, dist, true);
    _x = x;
    _y = y;
    _z = z;

    _queue.push_back(m);

    std::cout << "RAPID: " << m << "\n";
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
    setup_feed(m.z, _z, z, dist, true);
    _x = x;
    _y = y;
    _z = z;

    _queue.push_back(m);
    std::cout << "FEED: " << m << "\n";
}

void MoveQueue::go(std::function<void ()> func) {
    _cb = func;

    Move& m = _queue.front();

    _d->move(&m.x, &m.y, &m.z, this);

    _queue.pop_front();
}

void MoveQueue::setup(Movement& m, double o, double n, double dist, bool f)
{
    double d = o - n;
    m.direction = d < 0 ? 0 : 1;
    if (f) {
        m.direction = 1 - m.direction;
    }
    m.distance = fabs(d);
    m.velocity = (m.distance / dist) * _vel;
    m.acceleration = _acc;
}

void MoveQueue::setup_feed(Movement& m, double o, double n, double dist, bool f)
{
    double d = o - n;
    m.direction = d < 0 ? 0 : 1;
    if (f) {
        m.direction = 1 - m.direction;
    }
    m.distance = fabs(d);
    m.velocity = (d / dist) * _fvel;
    m.acceleration = 0;
}




