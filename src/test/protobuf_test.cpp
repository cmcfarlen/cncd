/* protobuf_test.cpp */

#include <iostream>
#include <fstream>

#include "mill.pb.h"

int main(int argc, char** argv)
{
    if (argc > 1) {
        std::fstream f(argv[1], std::ios::in | std::ios::binary);

        mill::CommandList clist;
        clist.ParseFromIstream(&f);

        for (int i = 0; i < clist.command_size(); i++) {
            const mill::Command& cmd = clist.command(i);

            switch (cmd.type()) {
                case mill::Command::ZERO:
                    std::cout << "ZERO\n";
                    break;
                case mill::Command::RAPIDTO:
                    std::cout << "RAPIDTO: " << cmd.x() << "," << cmd.y() << "," << cmd.z() << "\n";
                    break;
                case mill::Command::FEEDTO:
                    std::cout << "FEEDTO: " << cmd.x() << "," << cmd.y() << "," << cmd.z() << "\n";
                    break;
            }
        }
    }
    return 0;
}
