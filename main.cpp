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
#include "parport.h"
#include "parport_mill_driver.h"
#include "move_queue.h"
#include "gcode_parser.h"
#include "server.h"

#include "proto/mill.pb.h"

class TestHandler : public TimerHandler
{
public:
    double start_time;
    int count;

    TestHandler() : start_time(now())
    {}

    void tick()
    {
        count++;
    }

    void reset() {
        start_time = now();
        count = 0;
    }

    void report() {
        double end_time = now();

        printf("ticked %i times in %g seconds\n", count, end_time - start_time);
    }
};

static volatile int not_done = 1;

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
    ParPortMillDriver driver("/dev/parport0");
    MoveQueue q(&driver);

    q.move_to(0, 0, 40);

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
}

void calibration_test()
{
    init_realtime();
    ParPortMillDriver driver("/dev/parport0");
    MoveQueue q(&driver);

    q.calibrate();

    q.go([&driver] () {
            double* p = driver.position();
            std::cout << "position: " <<  p[0] << " " << p[1] << " " << p[2] << "\n";
            p = driver.max_position();
            std::cout << "max position: " <<  p[0] << " " << p[1] << " " << p[2] << "\nall done\n";
            });

    sleep_till_done([&]() { return q.done(); });
}

void timer_test()
{
    TestHandler h;
    TestHandler h2;
    TestHandler h3;
    Timer t(&h);
    Timer t2(&h2);
    Timer t3(&h3);

    h.reset();
    h2.reset();
    h3.reset();
    t.start(10000.0);
    t2.start(10000.0);
    t3.start(10000.0);

    stay_asleep(4);
    h.report();
    h2.report();
    h3.report();
}

void gcode_test(const std::string& path)
{
    GcodeParser p;

    p.parse(path);
}

void clear_axis(mill::MillState::Axis* a)
{
    a->set_direction(mill::MillState::Axis::IDLE);
    a->set_position(0);
    a->set_velocity(0);
    a->set_acceleration(0);
}

class ProtobufSession : public Session
{
public:
    ProtobufSession(int s, MoveQueue& q) : Session(s), millq(q)
    {
        q.set_position_callback([this] (double *p) { 
                send_state(p[0], p[1], p[2]);
                     });
    }

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
            std::string rdata(data, rc);
            mill::Request req;
            req.ParseFromString(rdata);

            if (req.has_command()) {
                std::cout << "got command\n";
                dispatch(req.command());
            }
            if (req.has_command_list()) {
                std::cout << "got command list\n";
                dispatch(req.command_list());
            }

            millq.go([this] () { send_complete(); });
            send_received();

        } else {
            std::cout << "recv returned " << rc << "\n";
            close();
        }
    }

    void dispatch(const mill::Command& cmd)
    {
        switch (cmd.type()) {
            case mill::Command::ZERO:
                std::cout << "ZERO\n";
                millq.zero();
                break;
            case mill::Command::RAPID:
                if (cmd.has_v()) {
                    std::cout << "RATE: " << cmd.v() << " ACC: " << cmd.a() << "\n";
                    millq.set_rapid_rate(cmd.v());
                    millq.set_acceleration(cmd.a());
                }
                std::cout << "RAPID: " << cmd.x() << "," << cmd.y() << "," << cmd.z() << "\n";
                millq.move(cmd.has_x() ? cmd.x() : millq.x(),
                        cmd.has_y() ? cmd.y() : millq.y(),
                        cmd.has_z() ? cmd.z() : millq.z());
                break;
            case mill::Command::RAPIDTO:
                if (cmd.has_v()) {
                    std::cout << "RATE: " << cmd.v() << " ACC: " << cmd.a() << "\n";
                    millq.set_rapid_rate(cmd.v());
                    millq.set_acceleration(cmd.a());
                }
                std::cout << "RAPIDTO: " << cmd.x() << "," << cmd.y() << "," << cmd.z() << "\n";
                millq.move_to(cmd.has_x() ? cmd.x() : millq.x(),
                        cmd.has_y() ? cmd.y() : millq.y(),
                        cmd.has_z() ? cmd.z() : millq.z());
                break;
            case mill::Command::FEEDTO:
                if (cmd.has_v()) {
                    std::cout << "FEEDRATE: " << cmd.v();
                    millq.set_feed_rate(cmd.v());
                }
                std::cout << "FEEDTO: " << cmd.x() << "," << cmd.y() << "," << cmd.z() << "\n";
                millq.feed_to(cmd.has_x() ? cmd.x() : millq.x(),
                        cmd.has_y() ? cmd.y() : millq.y(),
                        cmd.has_z() ? cmd.z() : millq.z());
                break;
            case mill::Command::STARTPOS:
                millq.set_start_position(cmd.has_x() ? cmd.x() : 0,
                                         cmd.has_y() ? cmd.y() : 0,
                                         cmd.has_z() ? cmd.z() : 0);
                std::cout << "STARTPOS: " << cmd.x() << "," << cmd.y() << "," << cmd.z() << "\n";
                break;
        }
    }

    void dispatch(const mill::CommandList& clist)
    {
        for (int i = 0; i < clist.command_size(); i++) {
            const mill::Command& cmd = clist.command(i);
            dispatch(cmd);
        }
    }

    void send_received()
    {
        mill::Response resp;
        mill::CommandReceived* recv = resp.mutable_command_received();

        recv->set_number(42);

        std::string out;
        resp.SerializeToString(&out);

        send(out.c_str(), out.length());
    }

    void send_complete()
    {
        mill::Response resp;
        mill::CommandComplete* recv = resp.mutable_command_complete();

        recv->set_number(42);

        std::string out;
        resp.SerializeToString(&out);

        send(out.c_str(), out.length());
    }

    void send_state(double x, double y, double z)
    {
        mill::Response resp;
        mill::MillState* st = resp.mutable_mill_state();

        mill::MillState::Axis* a = st->mutable_x();
        a->set_position(x);
        a = st->mutable_y();
        a->set_position(y);
        a = st->mutable_z();
        a->set_position(z);

        std::string out;
        resp.SerializeToString(&out);
        send(out.c_str(), out.length());
    }

private:
    MoveQueue& millq;
};

class ProtobufSessionFactory : public SessionFactory
{
public:
    ProtobufSessionFactory(MoveQueue& q)
        : _q(q)
    {}
    ~ProtobufSessionFactory() {}

    Session* session(int sock) {
        return new ProtobufSession(sock, _q);
    }
    void destroy_session(Session* s) {
        delete s;
    }
private:
    MoveQueue& _q;
};


void protobuf_test(int argc, char** argv)
{
    ParPortMillDriver driver("/dev/parport0");
    MoveQueue q(&driver);

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
                    if (cmd.has_v()) {
                        q.set_rapid_rate(cmd.v());
                        q.set_acceleration(cmd.a());
                    }
                    std::cout << "RAPIDTO: " << cmd.x() << "," << cmd.y() << "," << cmd.z() << "\n";
                    q.move_to(cmd.has_x() ? cmd.x() : q.x(),
                              cmd.has_y() ? cmd.y() : q.y(),
                              cmd.has_z() ? cmd.z() : q.z());
                    break;
                case mill::Command::FEEDTO:
                    if (cmd.has_v()) {
                        q.set_feed_rate(cmd.v());
                    }
                    std::cout << "FEEDTO: " << cmd.x() << "," << cmd.y() << "," << cmd.z() << "\n";
                    q.feed_to(cmd.has_x() ? cmd.x() : q.x(),
                              cmd.has_y() ? cmd.y() : q.y(),
                              cmd.has_z() ? cmd.z() : q.z());
                    break;
            }
        }
    }
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
    ParPortMillDriver driver("/dev/parport0");
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

void pport_test()
{
    ParPort port("/dev/parport0");

    Timer t([&port] () {
       int ctrl = port.status();
       if (ctrl & 0x40) {
           std::cout << "limit switch!\n";
       }
    });

    t.start(30);

    while (1) {
        sleep(100);
    }
}


int main(int argc, char** argv)
{
    //for (int i = 1; i < argc; i++) {
        //gcode_test(argv[i]);
    //}
    //

    //protobuf_test(argc, argv);
    server_test();
    //calibration_test();
    //pport_test();

    return 0;
}

