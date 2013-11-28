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

#include "real_parport.h"

class RealParPortPrivate
{
public:
    int fd;
    int data;
};

RealParPort::RealParPort(const std::string& dev)
    : _p(new RealParPortPrivate())
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

RealParPort::~RealParPort()
{
    if (_p->fd != -1) {
        ioctl(_p->fd, PPRELEASE);
        close(_p->fd);
    }
}

int RealParPort::data()
{
    return _p->data;
}

int RealParPort::data(int o)
{
    _p->data = o;
    ioctl(_p->fd, PPWDATA, &_p->data);
    return _p->data;
}

int RealParPort::merge(int o, int m)
{
    return data((_p->data & ~m) | (o & m));
}

int RealParPort::control()
{
    int ctrl;
    ioctl(_p->fd, PPRCONTROL, &ctrl);
    return ctrl;
}

int RealParPort::status()
{
    int ctrl;
    ioctl(_p->fd, PPRSTATUS, &ctrl);
    return ctrl;
}


