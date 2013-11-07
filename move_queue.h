/* move_queue.h */

#ifndef _GCODED_MOVE_QUEUE_H_
#define _GCODED_MOVE_QUEUE_H_

#include <functional>
#include <deque>
#include "axis.h"
#include "mill_driver.h"
#include "timer.h"

struct Move
{
    Move() : zero(0) {}
    int      zero; // its a zero move
    int      calibrate; // its a zero move
    Movement x;
    Movement y;
    Movement z;
};

inline std::ostream& operator<<(std::ostream& os, const Move& m)
{
    os << "{" << m.x << "," << m.y << "," << m.z << "}";
    return os;
}

class MoveQueue : public MillDriverCallback, public TimerHandler
{
public:
    MoveQueue(MillDriver* d);
    ~MoveQueue();

    void movement_complete();
    void limit_reached(char*);

    void zero();
    void calibrate();
    void set(double x, double y, double z);
    void move(double dx, double dy, double dz);
    void move_to(double x, double y, double z);
    void feed(double dx, double dy, double dz);
    void feed_to(double x, double y, double z);
    void setup(Movement& m, double o, double n, double dist);
    void setup_feed(Movement& m, double o, double n, double dist);

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

    void set_start_position(double x, double y, double z) {
        _sx = x;
        _sy = y;
        _sz = z;
    }

    void set_position_callback(std::function<void (double*)> f)
    {
        _pcb = f;
    }

    void clear_position_callback()
    {
        _pcb = std::function<void (double*)>();
    }

    void tick();

private:
    std::deque<Move> _queue;
    MillDriver*      _d;
    double _x, _y, _z;
    double _sx, _sy, _sz;
    double _vel;
    double _fvel;
    double _acc;
    bool   _done;
    std::function<void ()> _cb;
    std::function<void (double*)> _pcb;
    Timer  _update_timer;
};

#endif

