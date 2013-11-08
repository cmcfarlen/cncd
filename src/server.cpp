/* server.cpp */

#include <vector>
#include <map>
#include <set>

#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "server.h"


Session::~Session()
{}

void Session::on_connected()
{

}
void Session::on_closed()
{

}
void Session::on_data()
{

}

void Session::close()
{
    ::close(socket());
}

int Session::recv(char* data, size_t len)
{
    return ::recv(socket(), (void*)data, len, 0);
}

int Session::send(const char* data, size_t len)
{
    return ::send(socket(), (const void*)data, len, 0);
}

class ServerPrivate
{
public:
    int port;
    int listen_socket;

    SessionFactory* factory;
    std::map<int,Session*> sessions;
};

Server::Server(SessionFactory* f)
    : _p(new ServerPrivate())
{
    _p->factory = f;
}

void Server::listen(int port)
{
    struct sockaddr_in my_addr;
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        perror("socket");
    }
    int optval = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(port);

    if (bind(s, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1) {
        perror("bind");
    }

    ::listen(s, 10);

    _p->port = port;
    _p->listen_socket = s;
}

void Server::run()
{
    while (1) {
        struct pollfd fds[32];
        int fdcnt = 0;

        fds[0].fd = _p->listen_socket;
        fds[0].events = POLLIN | POLLHUP | POLLRDHUP;
        fds[0].revents = 0;
        fdcnt++;

        for (auto s: _p->sessions) {
            fds[fdcnt].fd = s.first;
            fds[fdcnt].events = POLLIN | POLLHUP | POLLRDHUP;
            fds[fdcnt].revents = 0;
            fdcnt++;
        }

        int rc;
        while ((rc = poll(fds, fdcnt, 10000)) == -1 && errno == EINTR);
        if (rc > 0) {
            for (int i = 0; i < fdcnt; i++) {
                short re = fds[i].revents;
                if (re & POLLIN) {
                    if (fds[i].fd == _p->listen_socket) {
                        sockaddr_in addr;
                        socklen_t addrlen;
                        int s = accept(_p->listen_socket, (struct sockaddr*)&addr, &addrlen);
                        Session* sess = _p->factory->session(s);
                        sess->on_connected();

                        _p->sessions[s] = sess;
                    } else {
                        auto sit = _p->sessions.find(fds[i].fd);
                        if (sit != _p->sessions.end()) {
                            (*sit).second->on_data();
                        }
                    }
                }
                if (re & POLLRDHUP) {
                    auto sit = _p->sessions.find(fds[i].fd);
                    if (sit != _p->sessions.end()) {
                        (*sit).second->on_closed();
                        _p->factory->destroy_session((*sit).second);
                        _p->sessions.erase(fds[i].fd);
                    }
                }
            }
        }
    }
}
