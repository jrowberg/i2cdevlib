/*
Copyright (c) 2009, Roger Light <roger@atchoo.org>
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

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>

#include <mqtt3.h>

typedef enum {
	stStart,
	stSocketOpened,
	stConnSent,
	stConnAckd,
	stSubSent,
	stSubAckd,
	stPause
} stateType;

static stateType state = stStart;

int handle_read(mqtt3_context *context)
{
	uint8_t buf;
	int rc;

	rc = read(context->sock, &buf, 1);
	printf("rc: %d\n", rc);
	if(rc == -1){
		printf("Error: %s\n", strerror(errno));
		return 1;
	}else if(rc == 0){
		return 2;
	}

	switch(buf&0xF0){
		case CONNACK:
			if(mqtt3_handle_connack(context)) return 3;
			state = stConnAckd;
			break;
		case SUBACK:
			if(mqtt3_handle_suback(context)) return 3;
			state = stSubAckd;
			break;
		case PINGREQ:
			if(mqtt3_handle_pingreq(context)) return 3;
			break;
		case PINGRESP:
			if(mqtt3_handle_pingresp(context)) return 3;
			break;
		case PUBACK:
			if(mqtt3_handle_puback(context)) return 3;
			break;
		case PUBCOMP:
			if(mqtt3_handle_pubcomp(context)) return 3;
			break;
		case PUBLISH:
			if(mqtt3_handle_publish(context)) return 0;
			break;
		case PUBREC:
			if(mqtt3_handle_pubrec(context)) return 3;
			break;
		case UNSUBACK:
			if(mqtt3_handle_unsuback(context)) return 3;
			break;
		default:
			printf("Unknown command: %s (%d)\n", mqtt3_command_to_string(buf&0xF0), buf&0xF0);
			break;
	}
	return 0;
}

void send_random(mqtt3_context *context, int length)
{
	int fd = open("/dev/urandom", O_RDONLY);
	uint8_t buf[length];

	if(fd >= 0){
		if(read(fd, buf, length) == length){
			mqtt3_write_bytes(context, buf, length);
		}
		close(fd);
	}
}

/* pselect loop test */
int main(int argc, char *argv[])
{
	struct timespec timeout;
	fd_set readfds, writefds;
	int fdcount;
	int run = 1;
	mqtt3_context context;
	char id[30];

	if(argc == 2){
		sprintf(id, "test%s", argv[1]);
	}else{
		sprintf(id, "test");
	}
	context.sock = mqtt3_socket_connect("127.0.0.1", 1883);
	if(context.sock == -1){
		return 1;
	}
	state = stSocketOpened;

	while(run){
		FD_ZERO(&readfds);
		FD_SET(context.sock, &readfds);
		FD_ZERO(&writefds);
		//FD_SET(0, &writefds);
		timeout.tv_sec = 1;
		timeout.tv_nsec = 0;

		fdcount = pselect(context.sock+1, &readfds, &writefds, NULL, &timeout, NULL);
		if(fdcount == -1){
			fprintf(stderr, "Error in pselect: %s\n", strerror(errno));
			run = 0;
		}else if(fdcount == 0){
			switch(state){
				case stSocketOpened:
					mqtt3_raw_connect(&context, id, true, 2, true, "will", "aargh", 60, true);
					state = stConnSent;
					break;
				case stConnSent:
					printf("Waiting for CONNACK\n");
					break;
				case stConnAckd:
	//				printf("CONNACK received\n");
	//				mqtt3_raw_subscribe(&context, false, "a/b/c", 0);
	//				state = stSubSent;
					send_random(&context, 100);
					break;
				case stSubSent:
					printf("Waiting for SUBACK\n");
					break;
				case stSubAckd:
					printf("SUBACK received\n");
					mqtt3_raw_publish(&context, 0, 0, 0, 1, "a/b/c", 5, (uint8_t *)"Roger");
					state = stPause;
					break;
				case stPause:
					//mqtt3_raw_disconnect(&context);
					printf("Pause\n");
					break;
				default:
					fprintf(stderr, "Error: Unknown state\n");
					break;
			}
		}else{
			printf("fdcount=%d\n", fdcount);

			if(FD_ISSET(context.sock, &readfds)){
				if(handle_read(&context)){
					fprintf(stderr, "Socket closed on remote side\n");
					mqtt3_socket_close(&context);
					run = 0;
				}
			}
		}
	}
	return 0;
}

