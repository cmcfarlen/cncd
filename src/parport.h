/* parport.h */

#ifndef _GCODED_PARPORT_H_
#define _GCODED_PARPORT_H_

#include <string>

class ParPort
{
public:
    ParPort() {}
    virtual ~ParPort() {}

    virtual int data() = 0;
    virtual int data(int o) = 0;
    virtual int merge(int o, int m) = 0;
    virtual int control() = 0;
    virtual int status() = 0;
};

#endif
