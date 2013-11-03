/* parport.h */

#ifndef _GCODED_PARPORT_H_
#define _GCODED_PARPORT_H_

#include <stdint.h>
#include <string>


class ParPortPrivate;
class ParPort
{
public:
    ParPort(const std::string& dev);
    ~ParPort();

    int data();
    int data(int o);
    int merge(int o, int m);
    int control();

private:
    ParPortPrivate* _p;
};

#endif
