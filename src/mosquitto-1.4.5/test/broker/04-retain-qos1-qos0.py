#!/usr/bin/env python

# Test whether a retained PUBLISH to a topic with QoS 1 is retained.
# Subscription is made with QoS 0 so the retained message should also have QoS
# 0.

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
connect_packet = mosq_test.gen_connect("retain-qos1-test", keepalive=keepalive)
connack_packet = mosq_test.gen_connack(rc=0)

mid = 6
publish_packet = mosq_test.gen_publish("retain/qos1/test", qos=1, mid=mid, payload="retained message", retain=True)
puback_packet = mosq_test.gen_puback(mid)
mid = 18
subscribe_packet = mosq_test.gen_subscribe(mid, "retain/qos1/test", 0)
suback_packet = mosq_test.gen_suback(mid, 0)
publish0_packet = mosq_test.gen_publish("retain/qos1/test", qos=0, payload="retained message", retain=True)

cmd = ['../../src/mosquitto', '-p', '1888']
broker = mosq_test.start_broker(filename=os.path.basename(__file__), cmd=cmd)

try:
    sock = mosq_test.do_client_connect(connect_packet, connack_packet)
    sock.send(publish_packet)

    if mosq_test.expect_packet(sock, "puback", puback_packet):
        sock.send(subscribe_packet)

        if mosq_test.expect_packet(sock, "suback", suback_packet):
            if mosq_test.expect_packet(sock, "publish0", publish0_packet):
                rc = 0
    sock.close()
finally:
    broker.terminate()
    broker.wait()
    if rc:
        (stdo, stde) = broker.communicate()
        print(stde)

exit(rc)

