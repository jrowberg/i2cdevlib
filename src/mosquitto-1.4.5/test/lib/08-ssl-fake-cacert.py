#!/usr/bin/env python

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

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
ssock = ssl.wrap_socket(sock, ca_certs="../ssl/all-ca.crt",
        keyfile="../ssl/server.key", certfile="../ssl/server.crt",
        server_side=True, ssl_version=ssl.PROTOCOL_TLSv1, cert_reqs=ssl.CERT_REQUIRED)
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

    conn.close()
except ssl.SSLError:
    # Expected error due to ca certs not matching.
    pass
finally:
    time.sleep(1.0)
    try:
        client.terminate()
    except OSError:
        pass
    client.wait()
    ssock.close()

if client.returncode == 0:
    exit(0)
else:
    exit(1)

