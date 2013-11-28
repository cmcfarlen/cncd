/* parport_mill_driver.h */

#ifndef _GCODED_PARPORT_MILL_DRIVER_H_
#define _GCODED_PARPORT_MILL_DRIVER_H_

#include "mill_driver.h"
#include "parport.h"

class ParPortMillDriverPrivate;
class ParPortMillDriver : public MillDriver
{
public:
    ParPortMillDriver(ParPort* port);
    virtual ~ParPortMillDriver();

    void move_absolute(double x, double y, double z, double vel, double acc, MillDriverCallback*);
    void move(Movement* x, Movement* y, Movement* z, MillDriverCallback* callback);
    void zero(const char* axis);
    void calibrate(MillDriverCallback*);

    double* position();
    double* max_position();
private:
    ParPortMillDriverPrivate* _p;
};

#endif
