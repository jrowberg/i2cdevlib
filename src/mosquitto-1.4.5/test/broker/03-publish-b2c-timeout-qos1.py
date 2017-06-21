#!/usr/bin/env python

# Test whether a SUBSCRIBE to a topic with QoS 2 results in the correct SUBACK packet.

import subprocess
import socket
import time
from os import environ

import inspect, os, sys
# From http://stackoverflow.com/questions/279237/python-import-a-module-from-a-folder
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

import mosq_test

rc = 1
mid = 3265
keepalive = 60
connect_packet = mosq_test.gen_connect("pub-qos1-timeout-test", keepalive=keepalive)
connack_packet = mosq_test.gen_connack(rc=0)

subscribe_packet = mosq_test.gen_subscribe(mid, "qos1/timeout/test", 1)
suback_packet = mosq_test.gen_suback(mid, 1)

mid = 1
publish_packet = mosq_test.gen_publish("qos1/timeout/test", qos=1, mid=mid, payload="timeout-message")
publish_dup_packet = mosq_test.gen_publish("qos1/timeout/test", qos=1, mid=mid, payload="timeout-message", dup=True)
puback_packet = mosq_test.gen_puback(mid)

broker = mosq_test.start_broker(filename=os.path.basename(__file__))

try:
    sock = mosq_test.do_client_connect(connect_packet, connack_packet)
    sock.send(subscribe_packet)

    if mosq_test.expect_packet(sock, "suback", suback_packet):
        pub = subprocess.Popen(['./03-publish-b2c-timeout-qos1-helper.py'])
        pub.wait()
        # Should have now received a publish command

        if mosq_test.expect_packet(sock, "publish", publish_packet):
            # Wait for longer than 5 seconds to get republish with dup set
            # This is covered by the 8 second timeout

            if mosq_test.expect_packet(sock, "dup publish", publish_dup_packet):
                sock.send(puback_packet)
                rc = 0

    sock.close()
finally:
    broker.terminate()
    broker.wait()
    if rc:
        (stdo, stde) = broker.communicate()
        print(stde)

exit(rc)

