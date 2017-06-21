#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <mosquitto.h>

struct msg_list{
	struct msg_list *next;
	struct mosquitto_message msg;
	bool sent;
};

struct sub{
	uint16_t mid;
	char *topic;
	int qos;
	bool complete;
};

struct sub subs[3];
struct msg_list *messages_received = NULL;
struct msg_list *messages_sent = NULL;
int sent_count = 0;
int received_count = 0;

void on_message(void *obj, const struct mosquitto_message *msg)
{
	struct msg_list *tail, *new_list;

	received_count++;

	new_list = malloc(sizeof(struct msg_list));
	if(!new_list){
		fprintf(stderr, "Error allocating list memory.\n");
		return;
	}
	new_list->next = NULL;
	if(!mosquitto_message_copy(&new_list->msg, msg)){
		if(messages_received){
			tail = messages_received;
			while(tail->next){
				tail = tail->next;
			}
			tail->next = new_list;
		}else{
			messages_received = new_list;
		}
	}else{
		free(new_list);
		return;
	}
}

void on_publish(void *obj, uint16_t mid)
{
	struct msg_list *tail = messages_sent;

	sent_count++;

	while(tail){
		if(tail->msg.mid == mid){
			tail->sent = true;
			return;
		}
		tail = tail->next;
	}

	fprintf(stderr, "ERROR: Invalid on_publish() callback for mid %d\n", mid);
}

void on_subscribe(void *obj, uint16_t mid, int qos_count, const uint8_t *granted_qos)
{
	int i;
	for(i=0; i<3; i++){
		if(subs[i].mid == mid){
			if(subs[i].complete){
				fprintf(stderr, "WARNING: Duplicate on_subscribe() callback for mid %d\n", mid);
			}
			subs[i].complete = true;
			return;
		}
	}
	fprintf(stderr, "ERROR: Invalid on_subscribe() callback for mid %d\n", mid);
}

void on_disconnect(void *obj)
{
	printf("Disconnected cleanly.\n");
}

void rand_publish(struct mosquitto *mosq, const char *topic, int qos)
{
	int fd = open("/dev/urandom", O_RDONLY);
	uint8_t buf[100];
	uint16_t mid;
	struct msg_list *new_list, *tail;

	if(fd >= 0){
		if(read(fd, buf, 100) == 100){
			if(!mosquitto_publish(mosq, &mid, topic, 100, buf, qos, false)){
				new_list = malloc(sizeof(struct msg_list));
				if(new_list){
					new_list->msg.mid = mid;
					new_list->msg.topic = strdup(topic);
					new_list->msg.payloadlen = 100;
					new_list->msg.payload = malloc(100);
					memcpy(new_list->msg.payload, buf, 100);
					new_list->msg.retain = false;
					new_list->next = NULL;
					new_list->sent = false;

					if(messages_sent){
						tail = messages_sent;
						while(tail->next){
							tail = tail->next;
						}
						tail->next = new_list;
					}else{
						messages_sent = new_list;
					}
				}
			}
		}
		close(fd);
	}
}

int main(int argc, char *argv[])
{
	struct mosquitto *mosq;
	int i;
	time_t start;

	mosquitto_lib_init();

	mosq = mosquitto_new("qos-test", NULL);
	mosquitto_log_init(mosq, MOSQ_LOG_ALL, MOSQ_LOG_STDOUT);
	mosquitto_message_callback_set(mosq, on_message);
	mosquitto_publish_callback_set(mosq, on_publish);
	mosquitto_subscribe_callback_set(mosq, on_subscribe);
	mosquitto_disconnect_callback_set(mosq, on_disconnect);

	mosquitto_connect(mosq, "127.0.0.1", 1883, 60, true);
	subs[0].topic = "qos-test/0";
	subs[0].qos = 0;
	subs[0].complete = false;
	subs[1].topic = "qos-test/1";
	subs[1].qos = 1;
	subs[1].complete = false;
	subs[2].topic = "qos-test/2";
	subs[2].qos = 2;
	subs[2].complete = false;
	mosquitto_subscribe(mosq, &subs[0].mid, subs[0].topic, subs[0].qos);
	mosquitto_subscribe(mosq, &subs[1].mid, subs[1].topic, subs[1].qos);
	mosquitto_subscribe(mosq, &subs[2].mid, subs[2].topic, subs[2].qos);

	for(i=0; i<1; i++){
		rand_publish(mosq, "qos-test/0", 0);
		rand_publish(mosq, "qos-test/0", 1);
		rand_publish(mosq, "qos-test/0", 2);
		rand_publish(mosq, "qos-test/1", 0);
		rand_publish(mosq, "qos-test/1", 1);
		rand_publish(mosq, "qos-test/1", 2);
		rand_publish(mosq, "qos-test/2", 0);
		rand_publish(mosq, "qos-test/2", 1);
		rand_publish(mosq, "qos-test/2", 2);
	}
	start = time(NULL);
	while(!mosquitto_loop(mosq, -1)){
		if(time(NULL)-start > 20){
			mosquitto_disconnect(mosq);
		}
	}

	mosquitto_destroy(mosq);

	mosquitto_lib_cleanup();

	printf("Sent messages: %d\n", sent_count);
	printf("Received messages: %d\n", received_count);
	return 0;
}

