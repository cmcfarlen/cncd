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

    def move_to(self, x, y, z):
        c = self.add(mill_pb2.Command.RAPIDTO)
        c.x = x
        c.y = y
        c.z = z
        return self


def offset(b):
    b.zero()
    b.move_to(0, 0, 50)


def box(b):
    b.zero()
    b.move_to(10, 0, 0)
    b.move_to(10, 10, 0)
    b.move_to(0, 10, 0)
    b.move_to(0, 0, 0)


def ladders(b):
    b.zero()
    z = -9
    for y in range(0, 50, 10):
        for x in range(0, 50, 10):
            b.move_to(x, y, z)
            b.move_to(x + 10, y, z)
            b.move_to(x + 10, y + 10, z)
            b.move_to(x, y + 10, z)
            b.move_to(x, y, 0)
    b.move_to(0, 0, 0)


def star(b):
    z = -19
    l = 10
    c = 10

    b.zero()
    b.move_to(0, 0, z)
    for t in numpy.linspace(0, 2 * math.pi, c):
        x = l * math.cos(t)
        y = l * math.sin(t)
        b.move_to(x, y, z)
        b.move_to(0, 0, -7)
        b.move_to(0, 0, z)
    b.move_to(0, 0, 0)


def send_moves():
    req = mill_pb2.Request()
    b = CommandBuilder(req)

    #star(b)
    offset(b)

    client_socket.send(req.SerializeToString())

send_moves()

while 1:
    data = client_socket.recv(4096)
    resp = mill_pb2.Response()
    resp.ParseFromString(data)
    print resp
    if resp.HasField('command_complete'):
        break
