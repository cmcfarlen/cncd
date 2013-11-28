/* fake_parport_test.cpp */


#include "fake_parport.h"

void config(Data& a, long range, long dirbits, long stepbits, long limitmask, long startp)
{
   a["step_range"] = range;
   a["dir_bits"] = dirbits;
   a["step_bits"] = stepbits;
   a["bit_mask"] = dirbits | stepbits;
   a["limit_mask"] = limitmask;
   a["start"] = startp;
}

int main(int argc, char** argv)
{
   Data cfg;

   config(cfg["x"], 20, 0x82, 0x41, 0x40, 10);
   config(cfg["y"], 20, 0x08, 0x04, 0x20, 10);
   config(cfg["z"], 20, 0x20, 0x10, 0x10, 10);

   std::cout << cfg << "\n";

   FakeParPort p(cfg);

   for (int i = 0; i < 12; i++) {
      p.data(0x01);
      p.data(0x00);
      std::cout << p;
   }

   for (int i = 0; i < 20; i++) {
      p.data(0x03);
      p.data(0x02);
      std::cout << p;
   }

   return 0;
}
