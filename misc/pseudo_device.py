#!/usr/bin/python3
#
# This script will create a pseudo terminal, and send dummy data over
# it for testing purposes. Note that pseuodo terminal is a unix thing,
# this script will not work on Windows.
#
# Currently this script only outputs ASCII(comma separated) data.
#
# Copyright © 2015 Hasan Yavuz Özderya
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

import os, pty, time, struct

def ascii_test(port):
    """Put ASCII test data through pseudo terminal."""
    print("\n")
    nc = 4 # number of channels
    for i in range(0, 1000):
        data = []
        for ci in range(0, nc):
            data.append(i*(ci+1))
        data = ",".join([str(num) for num in data])
        print("<< " + data, end="\r")
        os.write(port, bytes(data + "\r\n", 'ASCII'))
        time.sleep(0.1)

def float_test(port):
    """Put 32-bit float test data through pseudo terminal"""
    for i in range(0, 1000):
        f = float(i)/3.0
        data = struct.pack('f', f)
        os.write(port, data)
        import binascii
        time.sleep(0.1)

def run():
    # create the pseudo terminal
    master, slave = pty.openpty()

    master_name = os.ttyname(master)
    slave_name = os.ttyname(slave)
    print("Master terminal: {}\nSlave terminal: {}".format(master_name, slave_name))

    try:
        float_test(master)
    finally:
        # close the pseudo terminal files
        os.close(master)
        os.close(slave)

if __name__=="__main__":
    run()
