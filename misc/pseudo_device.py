#!/usr/bin/python3
#
# This script will create a pseudo terminal, and send dummy data over
# it for testing purposes. Note that pseuodo terminal is a unix thing,
# this script will not work on Windows.
#
# Currently this script only outputs ASCII(comma separated) data.
#
# Copyright © 2020 Hasan Yavuz Özderya
#
# This file is part of serialplot.
#
# serialplot is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# serialplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
#

import os, pty, time, struct, math

def ascii_test_str(port):
    text = """
1,2,3
2,4,6
3,8,11
1,2,3
-1,-1,-1
nana
0,0,0
1,na,na
    """
    while True:
        for line in text.splitlines():
            os.write(port, bytes(line+"\r\n", 'utf8'))
            time.sleep(1)

def ascii_test(port):
    """Put ASCII test data through pseudo terminal."""
    print("\n")
    nc = 4 # number of channels
    prefix = ""
    for i in range(0, 1000):
        data = []
        for ci in range(0, nc):
            data.append(i*(ci+1))
        data = ",".join([str(num) for num in data])
        print("<< " + data, end="\r")
        os.write(port, bytes(prefix + data + "\r\n", 'ASCII'))
        # os.write(port, bytes("##comment: test\r\n", 'ASCII'))
        time.sleep(0.1)

def float_test(port):
    """Put 32-bit float test data through pseudo terminal"""
    for i in range(0, 1000):
        f = float(i)/3.0
        data = struct.pack('f', f)
        os.write(port, data)
        import binascii
        time.sleep(0.1)

def float_sine(port):
    """Puts 2 channel sine and cosine wafeform through pseudo terminal
    continuously."""
    i = 0
    period = 200
    while True:
        sin = math.sin(2 * math.pi * i / period)
        # cos = math.sin(2 * math.pi * (i / period + 1 / 4))
        data = struct.pack('ff', sin, -sin)
        os.write(port, data)
        i = (i + 1) % period
        time.sleep(0.1)

def uint32_test(port, little):
    """Puts 32 bit unsigned integer data through pseudo terminal"""
    i = 0
    maxi = 200
    while True:
        data = struct.pack('>I', i)
        os.write(port, data)
        time.sleep(0.05)
        i = i+1 if i <= maxi else 0

def frame_test(port, fixed_size=False, hasChecksum=True):
    """Sends binary data in framed format."""
    SYNCWORD = [0xAA, 0xBB]
    NUMSAMPLES = 10
    SIZE = NUMSAMPLES * 4 # integer
    if fixed_size:
        HEADER = bytes(SYNCWORD)
    else:
        HEADER = bytes(SYNCWORD + [SIZE])
    i = 0
    checksum = 0
    bytesent = 0
    while True:
        if i > 100: i = 0
        if bytesent == 0: # beginning of a frame?
            os.write(port, HEADER)
        os.write(port, struct.pack('<I', i))
        bytesent += 4
        checksum += i
        i += 1
        if bytesent == SIZE: # end of a frame
            if hasChecksum:
                data = struct.pack('<I', checksum)[:1]
                os.write(port, data)
            checksum = 0
            bytesent = 0
        time.sleep(0.1)

def run():
    # create the pseudo terminal
    master, slave = pty.openpty()

    master_name = os.ttyname(master)
    slave_name = os.ttyname(slave)
    print("Master terminal: {}\nSlave terminal: {}".format(master_name, slave_name))

    try:
        # float_sine(master)
        # frame_test(master)
        ascii_test(master)
        # ascii_test_str(master)
    finally:
        # close the pseudo terminal files
        os.close(master)
        os.close(slave)

if __name__=="__main__":
    run()
