/* server.h */

#ifndef _GCODED_SERVER_H_
#define _GCODED_SERVER_H_

class Session
{
public:
    virtual ~Session();

    virtual void on_connected();
    virtual void on_closed();
    virtual void on_data();

    void close();
    int recv(char* data, size_t len);
    int send(const char* data, size_t len);

    int socket() { return _sock; }

protected:
    Session(int s) : _sock(s) {}
private:
    int _sock;
};

class SessionFactory
{
public:
    virtual ~SessionFactory() {}

    virtual Session* session(int sock) = 0;
    virtual void     destroy_session(Session*) = 0;
};

template <typename T>
class SessionFactoryImpl : public SessionFactory
{
public:
    Session* session(int sock) {
        return new T(sock);
    }
    void destroy_session(Session* s) {
        delete s;
    }
};

class ServerPrivate;
class Server
{
public:
    Server(SessionFactory* factory);
    ~Server() {}

    void listen(int port);

    void run();
private:
    ServerPrivate* _p;
};

#endif
