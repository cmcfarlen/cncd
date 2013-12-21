#!/usr/bin/python
import sys
import mill_pb2
import socket
import math
import numpy

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect(('localhost', 12333))

START_X = 50
START_Y = 50
START_Z = 135

PEN_Z = 10


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
    b.start(START_X, START_Y, START_Z)
    b.move(100, 0, 0)


def box(b):
    l = 30
    z = PEN_Z
    b.start(START_X, START_Y, START_Z)
    b.zero()
    b.move_to(l, 0, 0)
    b.move_to(l, 0, z)
    b.move_to(l, l, z)
    b.move_to(0, l, z)
    b.move_to(0, 0, z)
    b.move_to(0, 0, 0)


def ladders(b):
    z = PEN_Z

    b.start(START_X, START_Y, START_Z)
    b.zero()
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
    z = PEN_Z
    l = 10
    c = 10

    b.start(START_X, START_Y, START_Z)
    b.zero()
    for t in numpy.linspace(0, 2 * math.pi, c):
        x = l * math.cos(t)
        y = l * math.sin(t)
        b.move_to(0, 0, z)
        b.move_to(x, y, z)
        b.move_to(x, y, 0)
        b.move_to(0, 0, 0)


def heart_x(t):
    return 3.9 * math.pow(math.sin(t), 3)


def heart_y(t):
    return 3 * math.cos(t) - 1.2 * math.cos(2 * t) - 0.6 * math.cos(3 * t) - 0.2 * math.cos(4 * t)


def heart(b, ox=0):
    z = PEN_Z

    b.start(START_X + ox, START_Y, START_Z)
    b.zero()
    b.move_to(ox, 0, 0)
    x = heart_x(0)
    y = heart_y(0)
    b.move_to(x + ox, y, 0)
    for t in numpy.linspace(0, 2 * math.pi, 50):
        x = heart_x(t)
        y = heart_y(t)
        b.move_to(x + ox, y, z)

    b.move_to(x + ox, y, 0)


def circle(b, ox=0, oy=0):
    z = PEN_Z
    l = 10
    c = 10

    b.start(START_X + ox, START_Y + oy, START_Z)
    b.zero()
    x = l * math.cos(0)
    y = l * math.sin(0)
    b.move_to(x + ox, y + oy, 0)
    for t in numpy.linspace(0, 2 * math.pi, c * 10):
        b.move_to(x + ox, y + oy, z)
        x = l * math.cos(t) + ox
        y = l * math.sin(t) + oy

    b.move_to(ox, oy, -15)


def move_home(b):
    b.start(50, 50, 50)
    b.zero()


def move_zero(b):
    z = PEN_Z
    b.start(START_X, START_Y, START_Z)
    b.zero()
    b.move_to(0, 0, z)


def send_moves(m):
    req = mill_pb2.Request()
    b = CommandBuilder(req)

    m(b)

    client_socket.send(req.SerializeToString())

movemap = {'zero': move_zero, 'home': move_home, 'heart': heart, 'circle': circle, 'star': star, 'box': box, 'ladder': ladders}
move = move_zero

if len(sys.argv) > 1:
    print "performing: ", sys.argv[1]
    move = movemap[sys.argv[1]]

off = 0
send_moves(move)

while 1:
    data = client_socket.recv(4096)
    if len(data) == 0:
        break
    resp = mill_pb2.Response()
    resp.ParseFromString(data)
    if resp.HasField('mill_state'):
        print resp.mill_state.x.position, resp.mill_state.y.position, resp.mill_state.z.position
    if resp.HasField('command_complete'):
        break
