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
};

class MillDriver
{
public:
    virtual ~MillDriver() {};

    virtual void move(Movement* x, Movement* y, Movement* z, MillDriverCallback* callback) = 0;
};

#endif
