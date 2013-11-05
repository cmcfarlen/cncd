/* move_queue.h */

#ifndef _GCODED_MOVE_QUEUE_H_
#define _GCODED_MOVE_QUEUE_H_

#include <functional>
#include <deque>
#include "axis.h"
#include "mill_driver.h"

struct Move
{
    Movement x;
    Movement y;
    Movement z;
};

inline std::ostream& operator<<(std::ostream& os, const Move& m)
{
    os << "{" << m.x << "," << m.y << "," << m.z << "}";
    return os;
}

class MoveQueue : public MillDriverCallback
{
public:
    MoveQueue(MillDriver* d);
    ~MoveQueue();

    void movement_complete();

    void set(double x, double y, double z);
    void move_to(double x, double y, double z);
    void feed_to(double x, double y, double z);
    void setup(Movement& m, double o, double n, double dist, bool f = false);
    void setup_feed(Movement& m, double o, double n, double dist, bool f = false);

    void go(std::function<void ()> callback);
    bool done() { return _done; }

    double x() { return _x; }
    double y() { return _y; }
    double z() { return _z; }

    void set_rapid_rate(double v) {
        _vel = v;
    }
    void set_acceleration(double a) {
        _acc = a;
    }
    void set_feed_rate(double v) {
        _fvel = v;
    }

private:
    std::deque<Move> _queue;
    MillDriver*      _d;
    double _x, _y, _z;
    double _vel;
    double _fvel;
    double _acc;
    bool   _done;
    std::function<void ()> _cb;
};

#endif

