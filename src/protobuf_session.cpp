/* protobuf_session.cpp */

#include "protobuf_session.h"
#include "move_queue.h"


ProtobufSession::ProtobufSession(int s, MoveQueue& q) : Session(s), millq(q)
{
    q.set_position_callback([this] (double *p) { 
            send_state(p[0], p[1], p[2]);
            });
}

void ProtobufSession::on_connected()
{
    std::cout << "Connected!\n";
}

void ProtobufSession::on_closed()
{
    std::cout << "Closed!\n";
}

void ProtobufSession::on_data()
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

void ProtobufSession::dispatch(const mill::Command& cmd)
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

void ProtobufSession::dispatch(const mill::CommandList& clist)
{
    for (int i = 0; i < clist.command_size(); i++) {
        const mill::Command& cmd = clist.command(i);
        dispatch(cmd);
    }
}

void ProtobufSession::send_received()
{
    mill::Response resp;
    mill::CommandReceived* recv = resp.mutable_command_received();

    recv->set_number(42);

    std::string out;
    resp.SerializeToString(&out);

    send(out.c_str(), out.length());
}

void ProtobufSession::send_complete()
{
    mill::Response resp;
    mill::CommandComplete* recv = resp.mutable_command_complete();

    recv->set_number(42);

    std::string out;
    resp.SerializeToString(&out);

    send(out.c_str(), out.length());
}

void ProtobufSession::send_state(double x, double y, double z)
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

