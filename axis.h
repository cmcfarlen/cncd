/* axis.h */

#ifndef _GCODED_AXIS_H_
#define _GCODED_AXIS_H_

#include <functional>
#include <iostream>

struct Movement {
    Movement() : direction(0), distance(0), velocity(0), acceleration(0) {}

    int direction; //(0 toward motor(-), 1 away from motor(+))
    double distance;// (in mm)
    double velocity; //(in mm/sec)
    double acceleration; //(in mm/sec/sec)
    std::function<double (double)> acceleration_function;  // acceleration as a function of time (0 is the time at move start)
};

inline std::ostream& operator<<(std::ostream& o, const Movement& m)
{
    o << (m.direction ? "+":"-") << m.distance << "mm @ " << m.velocity << "mm/s";
    if (m.acceleration > 0) {
        o << " | " << m.acceleration << "mm/s/s";
    }
    return o;
}

struct MoveState {
    long steps;
    int direction;
    double start_time;
    double frequency;
    double velocity;
    double acceleration;
};

inline std::ostream& operator<<(std::ostream& o, const MoveState& m)
{
    o << "[" << m.steps << "," << (m.direction?"+":"-") << m.frequency << "," << m.velocity << "," << m.acceleration << "]";
    return o;
}

struct Axis {
    enum Limit {
        AtMin = 0x1,
        AtMax = 0x2
    };

    char axis[2];
    int  moving;
    int  limits; // Limit mask
    long pos;
    Movement current_move;
    MoveState move_state;
};

inline std::ostream& operator<<(std::ostream& o, const Axis& a)
{
    o << a.axis << " " << a.current_move << " - " << a.move_state;
    return o;
}



#endif
