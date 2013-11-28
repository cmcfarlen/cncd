/* real_parport.h */

#ifndef _REAL_PARPORT_H_
#define _REAL_PARPORT_H_

#include "parport.h"

class RealParPortPrivate;
class RealParPort : public ParPort
{
public:
    RealParPort(const std::string& dev);
    ~RealParPort();

    int data();
    int data(int o);
    int merge(int o, int m);
    int control();
    int status();

private:
    RealParPortPrivate* _p;
};


#endif
