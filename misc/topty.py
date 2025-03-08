#!/usr/bin/python3

# This scripts redirects standard input that you pipe into it to a
# pseudo port (slace terminal) that you can then open in
# SerialPlot. It allows you to plot the output of any command as long
# as SerialPlot can parse it.

import os, pty, sys, time

def run():
    # create the pseudo terminal
    master, slave = pty.openpty()

    slave_name = os.ttyname(slave)
    print("Slave terminal: {}".format(slave_name))
    sys.stdout.flush()

    try:
        for line in sys.stdin:
            sys.stdout.write(line)
            sys.stdout.flush()
            os.write(master, bytes(line, 'ASCII'))
    finally:
        # close the pseudo terminal files
        os.close(master)
        os.close(slave)

if __name__ == "__main__":
    run()
