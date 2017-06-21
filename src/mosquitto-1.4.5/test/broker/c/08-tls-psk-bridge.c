#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>

static int run = -1;
static int sent_mid;

void on_log(struct mosquitto *mosq, void *obj, int level, const char *str)
{
	printf("%s\n", str);
}

void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
	if(rc){
		exit(1);
	}else{
		mosquitto_publish(mosq, &sent_mid, "psk/test", strlen("message"), "message", 1, false);
	}
}

void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	if(mid == sent_mid){
		mosquitto_disconnect(mosq);
		run = 0;
	}else{
		exit(1);
	}
}

void on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
	run = rc;
}

int main(int argc, char *argv[])
{
	int rc;
	struct mosquitto *mosq;

	mosquitto_lib_init();

	mosq = mosquitto_new("08-tls-psk-bridge", true, NULL);
	mosquitto_tls_opts_set(mosq, 1, "tlsv1", NULL);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_disconnect_callback_set(mosq, on_disconnect);
	mosquitto_publish_callback_set(mosq, on_publish);
	mosquitto_log_callback_set(mosq, on_log);

	rc = mosquitto_connect(mosq, "localhost", 1890, 60);
	if(rc) return rc;

	while(run == -1){
		mosquitto_loop(mosq, -1, 1);
	}

	mosquitto_lib_cleanup();
	return run;
}
