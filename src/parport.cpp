/* parport.cpp */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/parport.h>
#include <linux/ppdev.h>

#include "parport.h"

class ParPortPrivate
{
public:
    int fd;
    int data;
};

ParPort::ParPort(const std::string& dev)
    : _p(new ParPortPrivate())
{
    _p->fd = open(dev.c_str(), O_RDWR);
    if (_p->fd == -1) {
        perror("parport open");
    } else {
        if (ioctl(_p->fd, PPCLAIM)) {
            perror("ioctl");
            close(_p->fd);
        }
    }
}

ParPort::~ParPort()
{
    if (_p->fd != -1) {
        ioctl(_p->fd, PPRELEASE);
        close(_p->fd);
    }
}

int ParPort::data()
{
    return _p->data;
}

int ParPort::data(int o)
{
    _p->data = o;
    ioctl(_p->fd, PPWDATA, &_p->data);
    return _p->data;
}

int ParPort::merge(int o, int m)
{
    return data((_p->data & ~m) | (o & m));
}

int ParPort::control()
{
    int ctrl;
    ioctl(_p->fd, PPRCONTROL, &ctrl);
    return ctrl;
}

int ParPort::status()
{
    int ctrl;
    ioctl(_p->fd, PPRSTATUS, &ctrl);
    return ctrl;
}


