#!/usr/bin/python3
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

import os, pty, time

def run():
    # create the pseudo terminal
    master, slave = pty.openpty()

    slave_name = os.ttyname(slave)
    print("Slave terminal: {}".format(slave_name))

    masterw = os.fdopen(master, 'w')
    masterr = os.fdopen(master, 'r')

    while True:
        line = masterr.read(4)
        print(">" + line)
        if (line.strip() == "data"):
            masterw.write("1,1\r\n")
        time.sleep(0.01)

if __name__ == "__main__":
    run()
