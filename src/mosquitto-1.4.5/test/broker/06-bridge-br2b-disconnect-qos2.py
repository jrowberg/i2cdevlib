#!/usr/bin/env python

# Does a bridge resend a QoS=1 message correctly after a disconnect?

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
client_id = socket.gethostname()+".bridge_sample"
connect_packet = mosq_test.gen_connect(client_id, keepalive=keepalive, clean_session=False, proto_ver=128+3)
connack_packet = mosq_test.gen_connack(rc=0)

mid = 1
subscribe_packet = mosq_test.gen_subscribe(mid, "bridge/#", 2)
suback_packet = mosq_test.gen_suback(mid, 2)

mid = 3
subscribe2_packet = mosq_test.gen_subscribe(mid, "bridge/#", 2)
suback2_packet = mosq_test.gen_suback(mid, 2)

mid = 4
subscribe3_packet = mosq_test.gen_subscribe(mid, "bridge/#", 2)
suback3_packet = mosq_test.gen_suback(mid, 2)

mid = 2
publish_packet = mosq_test.gen_publish("bridge/disconnect/test", qos=2, mid=mid, payload="disconnect-message")
publish_dup_packet = mosq_test.gen_publish("bridge/disconnect/test", qos=2, mid=mid, payload="disconnect-message", dup=True)
pubrec_packet = mosq_test.gen_pubrec(mid)
pubrel_packet = mosq_test.gen_pubrel(mid)
pubcomp_packet = mosq_test.gen_pubcomp(mid)

ssock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ssock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
ssock.settimeout(40)
ssock.bind(('', 1888))
ssock.listen(5)

broker = mosq_test.start_broker(filename=os.path.basename(__file__), port=1889)

try:
    (bridge, address) = ssock.accept()
    bridge.settimeout(20)

    if mosq_test.expect_packet(bridge, "connect", connect_packet):
        bridge.send(connack_packet)

        if mosq_test.expect_packet(bridge, "subscribe", subscribe_packet):
            bridge.send(suback_packet)

            pub = subprocess.Popen(['./06-bridge-br2b-disconnect-qos2-helper.py'])
            if pub.wait():
                exit(1)

            if mosq_test.expect_packet(bridge, "publish", publish_packet):
                bridge.close()

                (bridge, address) = ssock.accept()
                bridge.settimeout(20)

                if mosq_test.expect_packet(bridge, "connect", connect_packet):
                    bridge.send(connack_packet)

                    if mosq_test.expect_packet(bridge, "2nd subscribe", subscribe2_packet):
                        bridge.send(suback2_packet)

                        if mosq_test.expect_packet(bridge, "2nd publish", publish_dup_packet):
                            bridge.send(pubrec_packet)

                            if mosq_test.expect_packet(bridge, "pubrel", pubrel_packet):
                                bridge.close()

                                (bridge, address) = ssock.accept()
                                bridge.settimeout(20)

                                if mosq_test.expect_packet(bridge, "connect", connect_packet):
                                    bridge.send(connack_packet)

                                    if mosq_test.expect_packet(bridge, "3rd subscribe", subscribe3_packet):
                                        bridge.send(suback3_packet)

                                        if mosq_test.expect_packet(bridge, "2nd pubrel", pubrel_packet):
                                            bridge.send(pubcomp_packet)
                                            rc = 0

    bridge.close()
finally:
    try:
        bridge.close()
    except NameError:
        pass

    broker.terminate()
    broker.wait()
    if rc:
        (stdo, stde) = broker.communicate()
        print(stde)
    ssock.close()

exit(rc)

