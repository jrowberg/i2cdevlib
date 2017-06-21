/* Fudge a file description into a client instead of a socket connection so
 * that we can write out packets to a file.
 * See http://answers.launchpad.net/mosquitto/+question/123594
 * also http://answers.launchpad.net/mosquitto/+question/136821
 */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <mosquitto.h>
#include <mosquitto_internal.h>
#include <send_mosq.h>

int main(int argc, char *argv[])
{
	struct mosquitto *mosq;
	int fd;
	bool clean_session = true;
	int keepalive = 60;

	mosq = mosquitto_new("packetgen", NULL);
	if(!mosq){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}

	/* CONNECT */
	fd = open("mqtt.connect", O_CREAT|O_WRONLY, 00644);
	if(fd<0){
		fprintf(stderr, "Error: Unable to open mqtt.connect for writing.\n");
		return 1;
	}
	mosq->core.sock = fd;
	printf("_mosquitto_send_connect(): %d\n", _mosquitto_send_connect(mosq, keepalive, clean_session));
	printf("loop: %d\n", mosquitto_loop_write(mosq));
	close(fd);

	/* SUBSCRIBE */
	fd = open("mqtt.subscribe", O_CREAT|O_WRONLY, 00644);
	if(fd<0){
		fprintf(stderr, "Error: Unable to open mqtt.subscribe for writing.\n");
		return 1;
	}
	mosq->core.sock = fd;
	printf("_mosquitto_send_subscribe(): %d\n", _mosquitto_send_subscribe(mosq, NULL, false, "subscribe/topic", 2));
	printf("loop: %d\n", mosquitto_loop_write(mosq));
	close(fd);

	mosquitto_destroy(mosq);

	return 0;
}

