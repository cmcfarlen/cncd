/* fake_parport.h */

#ifndef _FAKE_PARPORT_H_
#define _FAKE_PARPORT_H_

#include "data.h"
#include "parport.h"

class FakeParPortPrivate;
class FakeParPort : public ParPort
{
public:
    FakeParPort(Data& config);
    ~FakeParPort();

    int data();
    int data(int o);
    int merge(int o, int m);
    int control();
    int status();

private:
    FakeParPortPrivate* _p;
};


#endif
