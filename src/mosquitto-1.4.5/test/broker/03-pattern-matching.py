#!/usr/bin/env python

import subprocess
import socket
import time

import inspect, os, sys
# From http://stackoverflow.com/questions/279237/python-import-a-module-from-a-folder
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

import mosq_test

def pattern_test(sub_topic, pub_topic):
    rc = 1
    keepalive = 60
    connect_packet = mosq_test.gen_connect("pattern-sub-test", keepalive=keepalive)
    connack_packet = mosq_test.gen_connack(rc=0)

    publish_packet = mosq_test.gen_publish(pub_topic, qos=0, payload="message")
    publish_retained_packet = mosq_test.gen_publish(pub_topic, qos=0, retain=True, payload="message")

    mid = 312
    subscribe_packet = mosq_test.gen_subscribe(mid, sub_topic, 0)
    suback_packet = mosq_test.gen_suback(mid, 0)

    mid = 234;
    unsubscribe_packet = mosq_test.gen_unsubscribe(mid, sub_topic)
    unsuback_packet = mosq_test.gen_unsuback(mid)

    broker = subprocess.Popen(['../../src/mosquitto', '-p', '1888'], stderr=subprocess.PIPE)

    try:
        time.sleep(0.5)

        sock = mosq_test.do_client_connect(connect_packet, connack_packet, timeout=20)
        sock.send(subscribe_packet)

        if mosq_test.expect_packet(sock, "suback", suback_packet):
            pub = subprocess.Popen(['./03-pattern-matching-helper.py', pub_topic])
            pub.wait()

            if mosq_test.expect_packet(sock, "publish", publish_packet):
                sock.send(unsubscribe_packet)

                if mosq_test.expect_packet(sock, "unsuback", unsuback_packet):
                    sock.send(subscribe_packet)

                    if mosq_test.expect_packet(sock, "suback", suback_packet):
                        if mosq_test.expect_packet(sock, "publish retained", publish_retained_packet):
                            rc = 0

        sock.close()
    finally:
        broker.terminate()
        broker.wait()
        if rc:
            (stdo, stde) = broker.communicate()
            print(stde)
            raise

    return rc

pattern_test("#", "test/topic")
pattern_test("#", "/test/topic")
pattern_test("foo/#", "foo/bar/baz")
pattern_test("foo/+/baz", "foo/bar/baz")
pattern_test("foo/+/baz/#", "foo/bar/baz")
pattern_test("foo/+/baz/#", "foo/bar/baz/bar")
pattern_test("foo/foo/baz/#", "foo/foo/baz/bar")
pattern_test("foo/#", "foo")
pattern_test("/#", "/foo")
pattern_test("test/topic/", "test/topic/")
pattern_test("test/topic/+", "test/topic/")
pattern_test("+/+/+/+/+/+/+/+/+/+/test", "one/two/three/four/five/six/seven/eight/nine/ten/test")

pattern_test("#", "test////a//topic")
pattern_test("#", "/test////a//topic")
pattern_test("foo/#", "foo//bar///baz")
pattern_test("foo/+/baz", "foo//baz")
pattern_test("foo/+/baz//", "foo//baz//")
pattern_test("foo/+/baz/#", "foo//baz")
pattern_test("foo/+/baz/#", "foo//baz/bar")
pattern_test("foo//baz/#", "foo//baz/bar")
pattern_test("foo/foo/baz/#", "foo/foo/baz/bar")
pattern_test("/#", "////foo///bar")

exit(0)

