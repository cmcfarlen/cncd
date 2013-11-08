/* mill_driver.h */

#ifndef _GCODED_MILL_DRIVER_H_
#define _GCODED_MILL_DRIVER_H_

#include "axis.h"

class MillDriver;

class MillDriverCallback
{
public:
    virtual ~MillDriverCallback() {}

    virtual void movement_complete() = 0;
    virtual void limit_reached(char* axis) = 0;
};

class MillDriver
{
public:
    virtual ~MillDriver() {};

    virtual void move_absolute(double x, double y, double z, double vel, double acc, MillDriverCallback*) = 0;
    virtual void move(Movement* x, Movement* y, Movement* z, MillDriverCallback* callback) = 0;
    virtual void zero(const char* axis) = 0;
    virtual void calibrate(MillDriverCallback*) = 0;

    virtual double* position() = 0;
    virtual double* max_position() = 0;

    virtual double rapid_velocity() { return 12; }
    virtual double max_velocity() { return 20; }
};

#endif
