#!/usr/bin/env python

# Connect a client with a will, then disconnect without DISCONNECT.

import subprocess
import socket
import time

import inspect, os, sys
# From http://stackoverflow.com/questions/279237/python-import-a-module-from-a-folder
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

import mosq_test

rc = 1
keepalive = 60
connect_packet = mosq_test.gen_connect("test-helper", keepalive=keepalive, will_topic="will/qos0/test", will_payload="will-message")
connack_packet = mosq_test.gen_connack(rc=0)

sock = mosq_test.do_client_connect(connect_packet, connack_packet)
rc = 0
sock.close()
    
exit(rc)

