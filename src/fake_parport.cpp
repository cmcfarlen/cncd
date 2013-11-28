/* fake_parport.cpp */

#include "fake_parport.h"

struct FakeAxis {
   FakeAxis(Data& cfg)
      : limit(cfg["step_range"].asLong()),
      dirbits(cfg["dir_bits"].asLong()),
      stepbits(cfg["step_bits"].asLong()),
      bitmask(dirbits | stepbits),
      limitmask(cfg["limit_mask"].asLong()),
      position(cfg["start"].asLong())
   {}

   int check_bits(int oldd, int newd, int st)
   {
      int os = oldd & stepbits;
      int ns = newd & stepbits;

      if (os != ns) {
         if (ns == 0) {
            int d = newd & dirbits ? -1 : 1;
            position += d;

            if (position < 0)
               position = 0;
            if (position > limit)
               position = limit;

         }
      }
      if (position == 0 ||
            position == limit) {
         return limitmask;
      }
      return 0;
   }

   void print_to(std::ostream& os) const {
      os << "(p:" << position << "/" << limit << ")";
   }

   int limit;
   int dirbits;
   int stepbits;
   int bitmask;
   int limitmask;
   int position;
};

class FakeParPortPrivate {
public:
   FakeParPortPrivate(Data& config)
      : x(config["x"]), y(config["y"]), z(config["z"]),
      data(0), control(0), status(0)
   {
   }

   FakeAxis x;
   FakeAxis y;
   FakeAxis z;

   int data;
   int status;
   int control;
};

FakeParPort::FakeParPort(Data& config) : _p(new FakeParPortPrivate(config))
{
}

FakeParPort::~FakeParPort()
{
}

int FakeParPort::data()
{
   return _p->data;
}

int FakeParPort::data(int o)
{
   _p->status =
      _p->x.check_bits(_p->data, o, _p->status) |
      _p->y.check_bits(_p->data, o, _p->status) |
      _p->z.check_bits(_p->data, o, _p->status);
   return _p->data = o;
}

int FakeParPort::merge(int o, int m)
{
   return data((_p->data & ~m) | (o & m));
}

int FakeParPort::control()
{
   return _p->control;
}

int FakeParPort::status()
{
   return _p->status;
}

void FakeParPort::print_to(std::ostream& os) const
{
   os << "(d:" << _p->data << "|s:" << _p->status << "|c:" << _p->control << ") ";
   os << "x:";
   _p->x.print_to(os);
   os << " y:";
   _p->y.print_to(os);
   os << " z:";
   _p->z.print_to(os);
   os << "\n";
}

