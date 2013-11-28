/* protobuf_session.h */

#ifndef _CNCD_PROTOBUF_SESSION_H_
#define _CNCD_PROTOBUF_SESSION_H_

#include "server.h"
#include "mill.pb.h"

class MoveQueue;

class ProtobufSession : public Session
{
public:
    ProtobufSession(int s, MoveQueue& q);

    void on_connected();

    void on_closed();

    void on_data();

    void dispatch(const mill::Command& cmd);

    void dispatch(const mill::CommandList& clist);

    void send_received();
    void send_complete();

    void send_state(double x, double y, double z);

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

#endif
