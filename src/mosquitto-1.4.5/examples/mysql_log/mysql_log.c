#include <signal.h>
#include <stdio.h>
#include <string.h>

#ifndef WIN32
#  include <unistd.h>
#else
#  include <process.h>
#  define snprintf sprintf_s
#endif

#include <mosquitto.h>
#include <mysql/mysql.h>

#define db_host "localhost"
#define db_username "mqtt_log"
#define db_password "password"
#define db_database "mqtt_log"
#define db_port 3306

#define db_query "INSERT INTO mqtt_log (topic, payload) VALUES (?,?)"

#define mqtt_host "localhost"
#define mqtt_port 1883

static int run = 1;
static MYSQL_STMT *stmt = NULL;

void handle_signal(int s)
{
	run = 0;
}

void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	MYSQL_BIND bind[2];

	memset(bind, 0, sizeof(bind));

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = message->topic;
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = message->payload;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);
}

int main(int argc, char *argv[])
{
	MYSQL *connection;
	my_bool reconnect = true;
	char clientid[24];
	struct mosquitto *mosq;
	int rc = 0;

	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	mysql_library_init(0, NULL, NULL);
	mosquitto_lib_init();

	connection = mysql_init(NULL);

	if(connection){
		mysql_options(connection, MYSQL_OPT_RECONNECT, &reconnect);

		connection = mysql_real_connect(connection, db_host, db_username, db_password, db_database, db_port, NULL, 0);

		if(connection){
			stmt = mysql_stmt_init(connection);

			mysql_stmt_prepare(stmt, db_query, strlen(db_query));

			memset(clientid, 0, 24);
			snprintf(clientid, 23, "mysql_log_%d", getpid());
			mosq = mosquitto_new(clientid, true, connection);
			if(mosq){
				mosquitto_connect_callback_set(mosq, connect_callback);
				mosquitto_message_callback_set(mosq, message_callback);


			    rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);

				mosquitto_subscribe(mosq, NULL, "#", 0);

				while(run){
					rc = mosquitto_loop(mosq, -1, 1);
					if(run && rc){
						sleep(20);
						mosquitto_reconnect(mosq);
					}
				}
				mosquitto_destroy(mosq);
			}
			mysql_stmt_close(stmt);

			mysql_close(connection);
		}else{
			fprintf(stderr, "Error: Unable to connect to database.\n");
			printf("%s\n", mysql_error(connection));
			rc = 1;
		}
	}else{
		fprintf(stderr, "Error: Unable to start mysql.\n");
		rc = 1;
	}

	mysql_library_end();
	mosquitto_lib_cleanup();

	return rc;
}

