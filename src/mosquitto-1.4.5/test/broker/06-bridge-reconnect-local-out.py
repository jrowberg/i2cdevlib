#!/usr/bin/env python

# Test whether a bridge topics work correctly after reconnection.
# Important point here is that persistence is enabled.

import os
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
connect_packet = mosq_test.gen_connect("bridge-reconnect-test", keepalive=keepalive)
connack_packet = mosq_test.gen_connack(rc=0)

mid = 180
subscribe_packet = mosq_test.gen_subscribe(mid, "bridge/#", 0)
suback_packet = mosq_test.gen_suback(mid, 0)
publish_packet = mosq_test.gen_publish("bridge/reconnect", qos=0, payload="bridge-reconnect-message")

try:
    os.remove('mosquitto.db')
except OSError:
    pass

cmd = ['../../src/mosquitto', '-p', '1888']
broker = mosq_test.start_broker(filename=os.path.basename(__file__), cmd=cmd)

local_cmd = ['../../src/mosquitto', '-c', '06-bridge-reconnect-local-out.conf']
local_broker = mosq_test.start_broker(cmd=local_cmd, filename=os.path.basename(__file__)+'_local1')
if os.environ.get('MOSQ_USE_VALGRIND') is not None:
    time.sleep(5)
else:
    time.sleep(0.5)
local_broker.terminate()
local_broker.wait()
if os.environ.get('MOSQ_USE_VALGRIND') is not None:
    time.sleep(5)
else:
    time.sleep(0.5)
local_broker = mosq_test.start_broker(cmd=local_cmd, filename=os.path.basename(__file__)+'_local2')
if os.environ.get('MOSQ_USE_VALGRIND') is not None:
    time.sleep(5)
else:
    time.sleep(0.5)

pub = None
try:
    sock = mosq_test.do_client_connect(connect_packet, connack_packet)
    sock.send(subscribe_packet)

    if mosq_test.expect_packet(sock, "suback", suback_packet):
        sock.send(subscribe_packet)

        if mosq_test.expect_packet(sock, "suback", suback_packet):
            pub = subprocess.Popen(['./06-bridge-reconnect-local-out-helper.py'], stdout=subprocess.PIPE)
            pub.wait()
            # Should have now received a publish command

            if mosq_test.expect_packet(sock, "publish", publish_packet):
                rc = 0
    sock.close()
finally:
    time.sleep(1)
    broker.terminate()
    broker.wait()
    if rc:
        (stdo, stde) = broker.communicate()
        print(stde)
    local_broker.terminate()
    local_broker.wait()
    if rc:
        (stdo, stde) = local_broker.communicate()
        print(stde)
        if pub:
            (stdo, stde) = pub.communicate()
            print(stdo)

    try:
        os.remove('mosquitto.db')
    except OSError:
        pass

exit(rc)

