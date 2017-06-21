#!/usr/bin/env python

# Test whether a client sends a correct PUBLISH to a topic with QoS 1, then responds correctly to a disconnect.

import inspect
import os
import subprocess
import socket
import sys
import time

# From http://stackoverflow.com/questions/279237/python-import-a-module-from-a-folder
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

import mosq_test

rc = 1
keepalive = 60
connect_packet = mosq_test.gen_connect("publish-qos1-test", keepalive=keepalive)
connack_packet = mosq_test.gen_connack(rc=0)

disconnect_packet = mosq_test.gen_disconnect()

mid = 1
publish_packet = mosq_test.gen_publish("pub/qos1/test", qos=1, mid=mid, payload="message")
publish_packet_dup = mosq_test.gen_publish("pub/qos1/test", qos=1, mid=mid, payload="message", dup=True)
puback_packet = mosq_test.gen_puback(mid)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.settimeout(10)
sock.bind(('', 1888))
sock.listen(5)

client_args = sys.argv[1:]
env = dict(os.environ)
env['LD_LIBRARY_PATH'] = '../../lib:../../lib/cpp'
try:
    pp = env['PYTHONPATH']
except KeyError:
    pp = ''
env['PYTHONPATH'] = '../../lib/python:'+pp

client = mosq_test.start_client(filename=sys.argv[1].replace('/', '-'), cmd=client_args, env=env)

try:
    (conn, address) = sock.accept()
    conn.settimeout(15)

    if mosq_test.expect_packet(conn, "connect", connect_packet):
        conn.send(connack_packet)

        if mosq_test.expect_packet(conn, "publish", publish_packet):
            # Disconnect client. It should reconnect.
            conn.close()

            (conn, address) = sock.accept()
            conn.settimeout(15)

            if mosq_test.expect_packet(conn, "connect", connect_packet):
                conn.send(connack_packet)

                if mosq_test.expect_packet(conn, "retried publish", publish_packet_dup):
                    conn.send(puback_packet)

                    if mosq_test.expect_packet(conn, "disconnect", disconnect_packet):
                        rc = 0

    conn.close()
finally:
    client.terminate()
    client.wait()
    sock.close()

exit(rc)
