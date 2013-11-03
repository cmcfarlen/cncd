/* parport_mill_driver.h */

#ifndef _GCODED_PARPORT_MILL_DRIVER_H_
#define _GCODED_PARPORT_MILL_DRIVER_H_

#include "mill_driver.h"
#include "parport.h"

class ParPortMillDriverPrivate;
class ParPortMillDriver : public MillDriver
{
public:
    ParPortMillDriver(const std::string& device);
    virtual ~ParPortMillDriver();

    void move(Movement* x, Movement* y, Movement* z, MillDriverCallback* callback);
private:
    ParPortMillDriverPrivate* _p;
};

#endif
