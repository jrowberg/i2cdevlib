#!/usr/bin/env python

# Test whether a retained PUBLISH is cleared when a zero length retained
# message is published to a topic.

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
connect_packet = mosq_test.gen_connect("retain-clear-test", keepalive=keepalive)
connack_packet = mosq_test.gen_connack(rc=0)

publish_packet = mosq_test.gen_publish("retain/clear/test", qos=0, payload="retained message", retain=True)
retain_clear_packet = mosq_test.gen_publish("retain/clear/test", qos=0, payload=None, retain=True)
mid_sub = 592
subscribe_packet = mosq_test.gen_subscribe(mid_sub, "retain/clear/test", 0)
suback_packet = mosq_test.gen_suback(mid_sub, 0)

mid_unsub = 593
unsubscribe_packet = mosq_test.gen_unsubscribe(mid_unsub, "retain/clear/test")
unsuback_packet = mosq_test.gen_unsuback(mid_unsub)

cmd = ['../../src/mosquitto', '-p', '1888']
broker = mosq_test.start_broker(filename=os.path.basename(__file__), cmd=cmd)

try:
    sock = mosq_test.do_client_connect(connect_packet, connack_packet, timeout=4)
    # Send retained message
    sock.send(publish_packet)
    # Subscribe to topic, we should get the retained message back.
    sock.send(subscribe_packet)

    if mosq_test.expect_packet(sock, "suback", suback_packet):
        if mosq_test.expect_packet(sock, "publish", publish_packet):
            # Now unsubscribe from the topic before we clear the retained
            # message.
            sock.send(unsubscribe_packet)

            if mosq_test.expect_packet(sock, "unsuback", unsuback_packet):
                # Now clear the retained message.
                sock.send(retain_clear_packet)

                # Subscribe to topic, we shouldn't get anything back apart
                # from the SUBACK.
                sock.send(subscribe_packet)
                if mosq_test.expect_packet(sock, "suback", suback_packet):
                    try:
                        retain_clear = sock.recv(256)
                    except socket.timeout:
                        # This is the expected event
                        rc = 0
                    else:
                        print("FAIL: Received unexpected message.")

    sock.close()
finally:
    broker.terminate()
    broker.wait()
    if rc:
        (stdo, stde) = broker.communicate()
        print(stde)

exit(rc)

