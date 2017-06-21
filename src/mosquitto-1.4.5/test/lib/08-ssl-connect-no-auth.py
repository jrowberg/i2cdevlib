#!/usr/bin/env python

# Test whether a client produces a correct connect and subsequent disconnect when using SSL.

# The client should connect to port 1888 with keepalive=60, clean session set,
# and client id 08-ssl-connect-no-auth
# It should use the CA certificate ssl/test-ca.crt for verifying the server.
# The test will send a CONNACK message to the client with rc=0. Upon receiving
# the CONNACK and verifying that rc=0, the client should send a DISCONNECT
# message. If rc!=0, the client should exit with an error.

import inspect
import os
import subprocess
import socket
import ssl
import sys
import time

# From http://stackoverflow.com/questions/279237/python-import-a-module-from-a-folder
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

import mosq_test

if sys.version < '2.7':
    print("WARNING: SSL not supported on Python 2.6")
    exit(0)

rc = 1
keepalive = 60
connect_packet = mosq_test.gen_connect("08-ssl-connect-no-auth", keepalive=keepalive)
connack_packet = mosq_test.gen_connack(rc=0)
disconnect_packet = mosq_test.gen_disconnect()

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
ssock = ssl.wrap_socket(sock, ca_certs="../ssl/all-ca.crt", keyfile="../ssl/server.key", certfile="../ssl/server.crt", server_side=True, ssl_version=ssl.PROTOCOL_TLSv1)
ssock.settimeout(10)
ssock.bind(('', 1888))
ssock.listen(5)

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
    (conn, address) = ssock.accept()
    conn.settimeout(100)

    if mosq_test.expect_packet(conn, "connect", connect_packet):
        conn.send(connack_packet)

        if mosq_test.expect_packet(conn, "disconnect", disconnect_packet):
            rc = 0

    conn.close()
finally:
    client.terminate()
    client.wait()
    ssock.close()

exit(rc)
