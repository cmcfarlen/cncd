/* parport_test.cpp */

#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include "timer.h"
#include "real_parport.h"

int main(int argc, char** argv)
{
    RealParPort port("/dev/parport0");

    Timer t([&port] () {
       int ctrl = port.status();
       //std::cout << std::hex << ctrl << std::dec << "\n";
       if (ctrl & 0x40) {
           std::cout << "X limit switch!\n";
       }
       if (ctrl & 0x20) {
           std::cout << "Y limit switch!\n";
       }
       if (ctrl & 0x10) {
           std::cout << "Z limit switch!\n";
       }
    });

    t.start(30);

    while (1) {
        sleep(100);
    }
}
