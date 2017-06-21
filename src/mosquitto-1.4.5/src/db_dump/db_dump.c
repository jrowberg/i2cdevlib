/*
Copyright (c) 2010-2012 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License v1.0
and Eclipse Distribution License v1.0 which accompany this distribution.
 
The Eclipse Public License is available at
   http://www.eclipse.org/legal/epl-v10.html
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.
 
Contributors:
   Roger Light - initial implementation and documentation.
*/

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include <mosquitto_broker.h>
#include <memory_mosq.h>
#include <persist.h>

static uint32_t db_version;
static int stats = 0;

static int _db_client_chunk_restore(struct mosquitto_db *db, FILE *db_fd)
{
	uint16_t i16temp, slen, last_mid;
	char *client_id = NULL;
	int rc = 0;
	time_t disconnect_t;

	read_e(db_fd, &i16temp, sizeof(uint16_t));
	slen = ntohs(i16temp);
	if(!slen){
		fprintf(stderr, "Error: Corrupt persistent database.");
		fclose(db_fd);
		return 1;
	}
	client_id = calloc(slen+1, sizeof(char));
	if(!client_id){
		fclose(db_fd);
		fprintf(stderr, "Error: Out of memory.");
		return 1;
	}
	read_e(db_fd, client_id, slen);
	if(!stats) printf("\tClient ID: %s\n", client_id);

	read_e(db_fd, &i16temp, sizeof(uint16_t));
	last_mid = ntohs(i16temp);
	if(!stats) printf("\tLast MID: %d\n", last_mid);

	if(db_version == 2){
		disconnect_t = time(NULL);
	}else{
		read_e(db_fd, &disconnect_t, sizeof(time_t));
		if(!stats) printf("\tDisconnect time: %ld\n", disconnect_t);
	}

	free(client_id);

	return rc;
error:
	fprintf(stderr, "Error: %s.", strerror(errno));
	if(db_fd >= 0) fclose(db_fd);
	if(client_id) free(client_id);
	return 1;
}

static int _db_client_msg_chunk_restore(struct mosquitto_db *db, FILE *db_fd)
{
	dbid_t i64temp, store_id;
	uint16_t i16temp, slen, mid;
	uint8_t qos, retain, direction, state, dup;
	char *client_id = NULL;

	read_e(db_fd, &i16temp, sizeof(uint16_t));
	slen = ntohs(i16temp);
	if(!slen){
		fprintf(stderr, "Error: Corrupt persistent database.");
		fclose(db_fd);
		return 1;
	}
	client_id = calloc(slen+1, sizeof(char));
	if(!client_id){
		fclose(db_fd);
		fprintf(stderr, "Error: Out of memory.");
		return 1;
	}
	read_e(db_fd, client_id, slen);
	if(!stats) printf("\tClient ID: %s\n", client_id);

	read_e(db_fd, &i64temp, sizeof(dbid_t));
	store_id = i64temp;
	if(!stats) printf("\tStore ID: %ld\n", (long )store_id);

	read_e(db_fd, &i16temp, sizeof(uint16_t));
	mid = ntohs(i16temp);
	if(!stats) printf("\tMID: %d\n", mid);

	read_e(db_fd, &qos, sizeof(uint8_t));
	if(!stats) printf("\tQoS: %d\n", qos);
	read_e(db_fd, &retain, sizeof(uint8_t));
	if(!stats) printf("\tRetain: %d\n", retain);
	read_e(db_fd, &direction, sizeof(uint8_t));
	if(!stats) printf("\tDirection: %d\n", direction);
	read_e(db_fd, &state, sizeof(uint8_t));
	if(!stats) printf("\tState: %d\n", state);
	read_e(db_fd, &dup, sizeof(uint8_t));
	if(!stats) printf("\tDup: %d\n", dup);

	free(client_id);

	return 0;
error:
	fprintf(stderr, "Error: %s.", strerror(errno));
	if(db_fd >= 0) fclose(db_fd);
	if(client_id) free(client_id);
	return 1;
}

static int _db_msg_store_chunk_restore(struct mosquitto_db *db, FILE *db_fd)
{
	dbid_t i64temp, store_id;
	uint32_t i32temp, payloadlen;
	uint16_t i16temp, slen, source_mid, mid;
	uint8_t qos, retain, *payload = NULL;
	char *source_id = NULL;
	char *topic = NULL;
	int rc = 0;
	bool binary;
	int i;

	read_e(db_fd, &i64temp, sizeof(dbid_t));
	store_id = i64temp;
	if(!stats) printf("\tStore ID: %ld\n", (long)store_id);

	read_e(db_fd, &i16temp, sizeof(uint16_t));
	slen = ntohs(i16temp);
	if(slen){
		source_id = calloc(slen+1, sizeof(char));
		if(!source_id){
			fclose(db_fd);
			fprintf(stderr, "Error: Out of memory.");
			return 1;
		}
		if(fread(source_id, 1, slen, db_fd) != slen){
			fprintf(stderr, "Error: %s.", strerror(errno));
			fclose(db_fd);
			free(source_id);
			return 1;
		}
		if(!stats) printf("\tSource ID: %s\n", source_id);
		free(source_id);
	}
	read_e(db_fd, &i16temp, sizeof(uint16_t));
	source_mid = ntohs(i16temp);
	if(!stats) printf("\tSource MID: %d\n", source_mid);

	read_e(db_fd, &i16temp, sizeof(uint16_t));
	mid = ntohs(i16temp);
	if(!stats) printf("\tMID: %d\n", mid);

	read_e(db_fd, &i16temp, sizeof(uint16_t));
	slen = ntohs(i16temp);
	if(slen){
		topic = calloc(slen+1, sizeof(char));
		if(!topic){
			fclose(db_fd);
			free(source_id);
			fprintf(stderr, "Error: Out of memory.");
			return 1;
		}
		if(fread(topic, 1, slen, db_fd) != slen){
			fprintf(stderr, "Error: %s.", strerror(errno));
			fclose(db_fd);
			free(source_id);
			free(topic);
			return 1;
		}
		if(!stats) printf("\tTopic: %s\n", topic);
		free(topic);
	}else{
		fprintf(stderr, "Error: Invalid msg_store chunk when restoring persistent database.");
		fclose(db_fd);
		free(source_id);
		return 1;
	}
	read_e(db_fd, &qos, sizeof(uint8_t));
	if(!stats) printf("\tQoS: %d\n", qos);
	read_e(db_fd, &retain, sizeof(uint8_t));
	if(!stats) printf("\tRetain: %d\n", retain);
	
	read_e(db_fd, &i32temp, sizeof(uint32_t));
	payloadlen = ntohl(i32temp);
	if(!stats) printf("\tPayload Length: %d\n", payloadlen);

	if(payloadlen){
		payload = malloc(payloadlen+1);
		if(!payload){
			fclose(db_fd);
			free(source_id);
			free(topic);
			fprintf(stderr, "Error: Out of memory.");
			return 1;
		}
		memset(payload, 0, payloadlen+1);
		if(fread(payload, 1, payloadlen, db_fd) != payloadlen){
			fprintf(stderr, "Error: %s.", strerror(errno));
			fclose(db_fd);
			free(source_id);
			free(topic);
			free(payload);
			return 1;
		}
		binary = false;
		for(i=0; i<payloadlen; i++){
			if(payload[i] == 0) binary = true;
		}
		if(binary == false && payloadlen<256){
			if(!stats) printf("\tPayload: %s\n", payload);
		}
		free(payload);
	}

	return rc;
error:
	fprintf(stderr, "Error: %s.", strerror(errno));
	if(db_fd >= 0) fclose(db_fd);
	if(source_id) free(source_id);
	if(topic) free(topic);
	return 1;
}

static int _db_retain_chunk_restore(struct mosquitto_db *db, FILE *db_fd)
{
	dbid_t i64temp, store_id;

	if(fread(&i64temp, sizeof(dbid_t), 1, db_fd) != 1){
		fprintf(stderr, "Error: %s.", strerror(errno));
		fclose(db_fd);
		return 1;
	}
	store_id = i64temp;
	if(!stats) printf("\tStore ID: %ld\n", (long int)store_id);
	return 0;
}

static int _db_sub_chunk_restore(struct mosquitto_db *db, FILE *db_fd)
{
	uint16_t i16temp, slen;
	uint8_t qos;
	char *client_id;
	char *topic;
	int rc = 0;

	read_e(db_fd, &i16temp, sizeof(uint16_t));
	slen = ntohs(i16temp);
	client_id = calloc(slen+1, sizeof(char));
	if(!client_id){
		fclose(db_fd);
		fprintf(stderr, "Error: Out of memory.");
		return 1;
	}
	read_e(db_fd, client_id, slen);
	if(!stats) printf("\tClient ID: %s\n", client_id);
	read_e(db_fd, &i16temp, sizeof(uint16_t));
	slen = ntohs(i16temp);
	topic = calloc(slen+1, sizeof(char));
	if(!topic){
		fclose(db_fd);
		fprintf(stderr, "Error: Out of memory.");
		free(client_id);
		return 1;
	}
	read_e(db_fd, topic, slen);
	if(!stats) printf("\tTopic: %s\n", topic);
	read_e(db_fd, &qos, sizeof(uint8_t));
	if(!stats) printf("\tQoS: %d\n", qos);
	free(client_id);
	free(topic);

	return rc;
error:
	fprintf(stderr, "Error: %s.", strerror(errno));
	if(db_fd >= 0) fclose(db_fd);
	return 1;
}

int main(int argc, char *argv[])
{
	FILE *fd;
	char header[15];
	int rc = 0;
	uint32_t crc;
	dbid_t i64temp;
	uint32_t i32temp, length;
	uint16_t i16temp, chunk;
	uint8_t i8temp;
	ssize_t rlen;
	struct mosquitto_db db;
	char *filename;
	long cfg_count = 0;
	long msg_store_count = 0;
	long client_msg_count = 0;
	long retain_count = 0;
	long sub_count = 0;
	long client_count = 0;

	if(argc == 2){
		filename = argv[1];
	}else if(argc == 3 && !strcmp(argv[1], "--stats")){
		stats = 1;
		filename = argv[2];
	}else{
		fprintf(stderr, "Usage: db_dump [--stats] <mosquitto db filename>\n");
		return 1;
	}
	memset(&db, 0, sizeof(struct mosquitto_db));
	fd = fopen(filename, "rb");
	if(!fd) return 0;
	read_e(fd, &header, 15);
	if(!memcmp(header, magic, 15)){
		if(!stats) printf("Mosquitto DB dump\n");
		// Restore DB as normal
		read_e(fd, &crc, sizeof(uint32_t));
		if(!stats) printf("CRC: %d\n", crc);
		read_e(fd, &i32temp, sizeof(uint32_t));
		db_version = ntohl(i32temp);
		if(!stats) printf("DB version: %d\n", db_version);

		while(rlen = fread(&i16temp, sizeof(uint16_t), 1, fd), rlen == 1){
			chunk = ntohs(i16temp);
			read_e(fd, &i32temp, sizeof(uint32_t));
			length = ntohl(i32temp);
			switch(chunk){
				case DB_CHUNK_CFG:
					cfg_count++;
					if(!stats) printf("DB_CHUNK_CFG:\n");
					if(!stats) printf("\tLength: %d\n", length);
					read_e(fd, &i8temp, sizeof(uint8_t)); // shutdown
					if(!stats) printf("\tShutdown: %d\n", i8temp);
					read_e(fd, &i8temp, sizeof(uint8_t)); // sizeof(dbid_t)
					if(!stats) printf("\tDB ID size: %d\n", i8temp);
					if(i8temp != sizeof(dbid_t)){
						fprintf(stderr, "Error: Incompatible database configuration (dbid size is %d bytes, expected %ld)",
								i8temp, sizeof(dbid_t));
						fclose(fd);
						return 1;
					}
					read_e(fd, &i64temp, sizeof(dbid_t));
					if(!stats) printf("\tLast DB ID: %ld\n", (long)i64temp);
					break;

				case DB_CHUNK_MSG_STORE:
					msg_store_count++;
					if(!stats) printf("DB_CHUNK_MSG_STORE:\n");
					if(!stats) printf("\tLength: %d\n", length);
					if(_db_msg_store_chunk_restore(&db, fd)) return 1;
					break;

				case DB_CHUNK_CLIENT_MSG:
					client_msg_count++;
					if(!stats) printf("DB_CHUNK_CLIENT_MSG:\n");
					if(!stats) printf("\tLength: %d\n", length);
					if(_db_client_msg_chunk_restore(&db, fd)) return 1;
					break;

				case DB_CHUNK_RETAIN:
					retain_count++;
					if(!stats) printf("DB_CHUNK_RETAIN:\n");
					if(!stats) printf("\tLength: %d\n", length);
					if(_db_retain_chunk_restore(&db, fd)) return 1;
					break;

				case DB_CHUNK_SUB:
					sub_count++;
					if(!stats) printf("DB_CHUNK_SUB:\n");
					if(!stats) printf("\tLength: %d\n", length);
					if(_db_sub_chunk_restore(&db, fd)) return 1;
					break;

				case DB_CHUNK_CLIENT:
					client_count++;
					if(!stats) printf("DB_CHUNK_CLIENT:\n");
					if(!stats) printf("\tLength: %d\n", length);
					if(_db_client_chunk_restore(&db, fd)) return 1;
					break;

				default:
					fprintf(stderr, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					fseek(fd, length, SEEK_CUR);
					break;
			}
		}
		if(rlen < 0) goto error;
	}else{
		fprintf(stderr, "Error: Unrecognised file format.");
		rc = 1;
	}

	fclose(fd);

	if(stats){
		printf("DB_CHUNK_CFG:        %ld\n", cfg_count);
		printf("DB_CHUNK_MSG_STORE:  %ld\n", msg_store_count);
		printf("DB_CHUNK_CLIENT_MSG: %ld\n", client_msg_count);
		printf("DB_CHUNK_RETAIN:     %ld\n", retain_count);
		printf("DB_CHUNK_SUB:        %ld\n", sub_count);
		printf("DB_CHUNK_CLIENT:     %ld\n", client_count);
	}
	return rc;
error:
	fprintf(stderr, "Error: %s.", strerror(errno));
	if(fd >= 0) fclose(fd);
	return 1;
}

