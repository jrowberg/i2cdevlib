#!/usr/bin/env python

import subprocess
import socket
import ssl
import sys
import time

if sys.version < '2.7':
    print("WARNING: SSL not supported on Python 2.6")
    exit(0)

if ssl.OPENSSL_VERSION_NUMBER < 0x10000000:
    print("WARNING: TLS-PSK not supported on OpenSSL < 1.0")
    exit(0)


import inspect, os, sys
# From http://stackoverflow.com/questions/279237/python-import-a-module-from-a-folder
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

import mosq_test

env = dict(os.environ)
env['LD_LIBRARY_PATH'] = '../../lib:../../lib/cpp'
try:
    pp = env['PYTHONPATH']
except KeyError:
    pp = ''
env['PYTHONPATH'] = '../../lib/python:'+pp


rc = 1
keepalive = 60
connect_packet = mosq_test.gen_connect("no-psk-test-client", keepalive=keepalive)
connack_packet = mosq_test.gen_connack(rc=0)

mid = 1
subscribe_packet = mosq_test.gen_subscribe(mid, "psk/test", 0)
suback_packet = mosq_test.gen_suback(mid, 0)

publish_packet = mosq_test.gen_publish(topic="psk/test", payload="message", qos=0)

bridge_cmd = ['../../src/mosquitto', '-c', '08-tls-psk-bridge.conf2']
broker = mosq_test.start_broker(filename=os.path.basename(__file__))
bridge = mosq_test.start_broker(filename=os.path.basename(__file__)+'_bridge', cmd=bridge_cmd, port=1890)

pub = None
try:
    sock = mosq_test.do_client_connect(connect_packet, connack_packet, timeout=30)
    sock.send(subscribe_packet)

    if mosq_test.expect_packet(sock, "suback", suback_packet):
        pub = subprocess.Popen(['./c/08-tls-psk-bridge.test'], env=env, stdout=subprocess.PIPE)
        if pub.wait():
            raise ValueError
            exit(1)

        if mosq_test.expect_packet(sock, "publish", publish_packet):
            rc = 0
    sock.close()
finally:
    time.sleep(1)
    broker.terminate()
    broker.wait()
    time.sleep(1)
    bridge.terminate()
    bridge.wait()
    if rc:
        (stdo, stde) = broker.communicate()
        print(stde)
        (stdo, stde) = bridge.communicate()
        print(stde)
        if pub:
            (stdo, stde) = pub.communicate()
            print(stdo)


exit(rc)

