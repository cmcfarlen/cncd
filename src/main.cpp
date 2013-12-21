/* main.cpp */

#include <fstream>

#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "timer.h"
#include "real_parport.h"
#include "fake_parport.h"
#include "parport_mill_driver.h"
#include "move_queue.h"
#include "gcode_parser.h"
#include "server.h"

#include "protobuf_session.h"

#include "mill.pb.h"


Data load_config(const char* fname)
{
    Data c;
    std::ifstream fs(fname);
    c.parse(fs);
    std::cout << "config!\n" << c << "\n";
    return c;
}

template <typename F>
void sleep_till_done(F d)
{
    while (!d()) {
        sleep(1);
    }
}

void init_realtime()
{
    if (geteuid() != 0) {
        printf("need to be root yo!\n");
        exit(1);
    }

    // rt goodness
    struct sched_param parms;
    memset(&parms, 0, sizeof(parms));
    parms.sched_priority = 10;
    if (sched_setscheduler(getpid(), SCHED_FIFO, &parms) == -1) {
        perror("sched_setscheduler");
    }

    // ... prealloc stack
    // ... memlocking
}

void move_test()
{
    init_realtime();
    Data c = load_config("./simulate.conf");
    //RealParPort port("/dev/parport0");
    FakeParPort port(c["machine"]);
    ParPortMillDriver driver(&port, c["machine"]);
    MoveQueue q(&driver);

    std::cout << port << "\n";

    q.set_position_callback([&port] (double* p) {
    std::cout << "x: " << p[0] << " y: " << p[1] << " z: " << p[2] << "\n";
    std::cout << port << "\n";
            });

    q.move_to(5, 0, 0);
    q.move_to(0, 0, 0);

    /*
    double z = -9;
    for (double y = 0; y < 50; y += 10) {
        for (double x = 0; x < 50; x += 10) {
            q.move_to(x, y, z);
            q.move_to(x+10, y, z);
            q.move_to(x+10, y+10, z);
            q.move_to(x, y+10, z);
            q.move_to(x, y, 0);
        }
        q.move_to(0, y, 0);
    }

    q.move_to(0, 0, 0);
    */

    //q.move_to(10, 0, 0);
    //q.move_to(10, 10, 0);
    //q.move_to(0, 10, 0);
    //q.move_to(0, 0, 0);

    q.go([] () { std::cout << " all done\n"; });

    sleep_till_done([&]() { return q.done(); });
    std::cout << port << "\n";
}

void calibration_test()
{
    init_realtime();
    Data c = load_config("./simulate.conf");
    //RealParPort port("/dev/parport0");
    FakeParPort port(c["machine"]);
    ParPortMillDriver driver(&port, c["machine"]);
    MoveQueue q(&driver);

    q.calibrate();

    q.set_position_callback([&port] (double* p) {
    std::cout << "x: " << p[0] << " y: " << p[1] << " z: " << p[2] << "\n";
    std::cout << port << "\n";
            });

    q.go([&driver] () {
            double* p = driver.position();
            std::cout << "position: " <<  p[0] << " " << p[1] << " " << p[2] << "\n";
            p = driver.max_position();
            std::cout << "max position: " <<  p[0] << " " << p[1] << " " << p[2] << "\nall done\n";
            });

    sleep_till_done([&]() { return q.done(); });
}


class TestSession : public Session
{
public:
    TestSession(int s) : Session(s)
    {}

    void on_connected()
    {
        std::cout << "Connected!\n";
    }

    void on_closed()
    {
        std::cout << "Closed!\n";
    }

    void on_data()
    {
        std::cout << "Data!\n";

        char data[4096];

        int rc = recv(data, sizeof(data));

        if (rc > 0) {
            data[rc] = 0;
            std::cout << "read " << rc << " bytes " << data << "\n";
            send(data, rc);
        } else {
            std::cout << "recv returned " << rc << "\n";
            close();
        }
    }
};


void server_test()
{
    init_realtime();
    Data c = load_config("./simulate.conf");
    //RealParPort port("/dev/parport0");
    FakeParPort port(c["machine"]);
    ParPortMillDriver driver(&port, c["machine"]);

    MoveQueue millq(&driver);
    ProtobufSessionFactory factory(millq);

    Server server(&factory);

    server.listen(12345);

    millq.calibrate();
    millq.go([&driver] () {
            double* p = driver.position();
            std::cout << "position: " <<  p[0] << " " << p[1] << " " << p[2] << "\n";
            p = driver.max_position();
            std::cout << "max position: " <<  p[0] << " " << p[1] << " " << p[2] << "\nall done\n";
            });

    server.run();
}


int main(int argc, char** argv)
{
    server_test();
    //move_test();
    //calibration_test();

    return 0;
}

