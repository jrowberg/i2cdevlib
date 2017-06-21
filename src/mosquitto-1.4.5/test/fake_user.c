/*
Copyright (c) 2009,2010, Roger Light <roger@atchoo.org>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of mosquitto nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <process.h>
#define snprintf sprintf_s
#endif

#include <mosquitto.h>

void my_connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	char topic[100];

	if(!result){
		snprintf(topic, 100, "fake/%d", getpid()%100);
		mosquitto_subscribe(mosq, NULL, topic, rand()%3);
	}
}

int main(int argc, char *argv[])
{
	char id[30];
	char *host = "localhost";
	int port = 1883;
	int keepalive = 60;
	bool clean_session = false;
	struct mosquitto *mosq = NULL;
	
	void *will_payload = NULL;
	long will_payloadlen = 0;
	int will_qos = 0;
	bool will_retain = false;
	char will_topic[100], topic[100];
	int pid;

	pid = getpid();

	srand(pid);
	snprintf(id, 30, "fake_user_%d", pid);

	mosquitto_lib_init();
	mosq = mosquitto_new(id, clean_session, NULL);
	if(!mosq){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}

	if(rand()%5 == 0){
		snprintf(will_topic, 100, "fake/wills/%d", rand()%100);
		if(mosquitto_will_set(mosq, will_topic, will_payloadlen, will_payload, will_qos, will_retain)){
			fprintf(stderr, "Error: Problem setting will.\n");
			return 1;
		}
	}
	mosquitto_connect_callback_set(mosq, my_connect_callback);
	while(1){
		clean_session = rand()%10==0?false:true;

		if(mosquitto_connect(mosq, host, port, keepalive)){
			fprintf(stderr, "Unable to connect.\n");
			return 1;
		}
		mosquitto_subscribe(mosq, NULL, "#", 0);

		while(!mosquitto_loop(mosq, -1, 5)){
			if(rand()%100==0){
				snprintf(topic, 100, "fake/%d", rand()%100);
				mosquitto_publish(mosq, NULL, topic, 10, "0123456789", rand()%3, rand()%2);
			}
			if(rand()%50==0){
				mosquitto_disconnect(mosq);
			}
		}
		sleep(10);
	}
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return 0;
}

