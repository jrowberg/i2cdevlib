#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>

static int run = -1;

void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
	exit(1);
}

int main(int argc, char *argv[])
{
	int rc;
	struct mosquitto *mosq;

	mosquitto_lib_init();

	mosq = mosquitto_new("08-ssl-connect-crt-auth", true, NULL);
	mosquitto_tls_opts_set(mosq, 1, "tlsv1", NULL);
	mosquitto_tls_set(mosq, "../ssl/test-fake-root-ca.crt", NULL, "../ssl/client.crt", "../ssl/client.key", NULL);
	mosquitto_connect_callback_set(mosq, on_connect);

	rc = mosquitto_connect(mosq, "localhost", 1888, 60);

	rc = mosquitto_loop_forever(mosq, -1, 1);
	if(rc == MOSQ_ERR_ERRNO && errno == EPROTO){
		return 0;
	}else{
		return 1;
	}
}

