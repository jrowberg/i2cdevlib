#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>

static int run = -1;
int main(int argc, char *argv[])
{
	int rc;
	struct mosquitto *mosq;

	mosquitto_lib_init();

	mosq = mosquitto_new("01-will-unpwd-set", true, NULL);
	mosquitto_username_pw_set(mosq, "oibvvwqw", "#'^2hg9a&nm38*us");
	mosquitto_will_set(mosq, "will-topic", strlen("will message"), "will message", 2, false);

	rc = mosquitto_connect(mosq, "localhost", 1888, 60);

	while(run == -1){
		mosquitto_loop(mosq, -1, 1);
	}

	mosquitto_lib_cleanup();
	return run;
}
