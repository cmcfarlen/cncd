#!/usr/bin/python
import mill_pb2
import socket
import math
import numpy

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect(('localhost', 12345))


class CommandBuilder:
    def __init__(self, r):
        self._r = r
        self._x = 0.
        self._y = 0.
        self._z = 0.

    def add(self, ctype):
        c = self._r.command_list.command.add()
        c.type = ctype
        return c

    def zero(self):
        self.add(mill_pb2.Command.ZERO)
        return self

    def start(self, x, y, z):
        c = self.add(mill_pb2.Command.STARTPOS)
        c.x = x
        c.y = y
        c.z = z
        return self

    def move_to(self, x, y, z):
        c = self.add(mill_pb2.Command.RAPIDTO)
        c.x = x
        c.y = y
        c.z = z
        return self

    def move(self, x, y, z):
        c = self.add(mill_pb2.Command.RAPID)
        c.x = x
        c.y = y
        c.z = z
        return self


def offset(b):
    #b.zero()
    #b.move(0, 0, 1)
    b.start(50, 50, 100)
    b.move(100, 0, 0)


def box(b):
    b.start(50, 50, 130)
    b.move_to(10, 0, 0)
    b.move_to(10, 10, 0)
    b.move_to(0, 10, 0)
    b.move_to(0, 0, 0)


def ladders(b):
    b.start(50, 50, 125)
    b.zero()
    z = 5
    for y in range(0, 30, 10):
        for x in range(0, 30, 10):
            b.move_to(x, y, 0)
            b.move_to(x, y, z)
            b.move_to(x + 10, y, z)
            b.move_to(x + 10, y + 10, z)
            b.move_to(x, y + 10, z)
            b.move_to(x + 10, y, 0)
    b.move_to(0, 0, -15)


def star(b):
    z = 4.5
    l = 10
    c = 10

    b.start(50, 50, 125)
    b.zero()
    for t in numpy.linspace(0, 2 * math.pi, c):
        x = l * math.cos(t)
        y = l * math.sin(t)
        b.move_to(0, 0, z)
        b.move_to(x, y, z)
        b.move_to(x, y, 0)
        b.move_to(0, 0, 0)


def circle(b, ox, oy):
    z = 4.5
    l = 10
    c = 10

    b.start(50, 50, 125)
    b.zero()
    x = l * math.cos(0)
    y = l * math.sin(0)
    b.move_to(x, y, 0)
    for t in numpy.linspace(0, 2 * math.pi, c * 10):
        b.move_to(x, y, z)
        x = l * math.cos(t) + ox
        y = l * math.sin(t) + oy

    b.move_to(0, 0, -15)


def send_moves(o):
    req = mill_pb2.Request()
    b = CommandBuilder(req)

    circle(b, o, 0)
    #ladders(b)
    #star(b)
    #offset(b)
    #box(b)

    client_socket.send(req.SerializeToString())

off = 0
send_moves(off)

while 1:
    data = client_socket.recv(4096)
    if len(data) == 0:
        break
    resp = mill_pb2.Response()
    resp.ParseFromString(data)
    if resp.HasField('mill_state'):
        print resp.mill_state.x.position, resp.mill_state.y.position, resp.mill_state.z.position
    if resp.HasField('command_complete'):
        if off < 30:
            off += 5
            send_moves(off)
