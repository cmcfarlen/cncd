/* fake_parport.h */

#ifndef _FAKE_PARPORT_H_
#define _FAKE_PARPORT_H_


#include <iostream>
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

    void print_to(std::ostream& os) const;

private:
    FakeParPortPrivate* _p;
};

inline std::ostream& operator<<(std::ostream& os, const FakeParPort& p)
{
    p.print_to(os);
    return os;
}


#endif
