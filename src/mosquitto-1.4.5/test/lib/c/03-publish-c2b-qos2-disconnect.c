#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>

static int run = -1;
static int first_connection = 1;

void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
	if(rc){
		exit(1);
	}else{
		if(first_connection == 1){
			mosquitto_publish(mosq, NULL, "pub/qos2/test", strlen("message"), "message", 2, false);
			first_connection = 0;
		}
	}
}

void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	mosquitto_disconnect(mosq);
}

void on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
	if(rc){
		mosquitto_reconnect(mosq);
	}else{
		run = 0;
	}
}

int main(int argc, char *argv[])
{
	int rc;
	struct mosquitto *mosq;

	mosquitto_lib_init();

	mosq = mosquitto_new("publish-qos2-test", true, NULL);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_disconnect_callback_set(mosq, on_disconnect);
	mosquitto_publish_callback_set(mosq, on_publish);
	mosquitto_message_retry_set(mosq, 3);

	rc = mosquitto_connect(mosq, "localhost", 1888, 60);

	while(run == -1){
		mosquitto_loop(mosq, 300, 1);
	}

	mosquitto_lib_cleanup();
	return run;
}
