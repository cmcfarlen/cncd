/* axis.h */

#ifndef _GCODED_AXIS_H_
#define _GCODED_AXIS_H_

#include <iostream>

struct Movement {
    int direction; //(0 one-way, 1 the other)
    double distance;// (in mm)
    double velocity; //(in mm/sec)
    double acceleration; //(in mm/sec/sec)
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
    char axis[2];
    int  moving;
    Movement current_move;
    MoveState move_state;
};

inline std::ostream& operator<<(std::ostream& o, const Axis& a)
{
    o << a.axis << " " << a.current_move << " - " << a.move_state;
    return o;
}



#endif
