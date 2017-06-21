/*
Copyright (c) 2013,2014 Roger Light <roger@atchoo.org>

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

#ifdef WITH_SRV
#  include <ares.h>

#  include <arpa/nameser.h>
#  include <stdio.h>
#  include <string.h>
#endif

#include "logging_mosq.h"
#include "memory_mosq.h"
#include "mosquitto_internal.h"
#include "mosquitto.h"

#ifdef WITH_SRV
static void srv_callback(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
{   
	struct mosquitto *mosq = arg;
	struct ares_srv_reply *reply = NULL;
	if(status == ARES_SUCCESS){
		status = ares_parse_srv_reply(abuf, alen, &reply);
		if(status == ARES_SUCCESS){
			// FIXME - choose which answer to use based on rfc2782 page 3. */
			mosquitto_connect(mosq, reply->host, reply->port, mosq->keepalive);
		}
	}else{
		_mosquitto_log_printf(mosq, MOSQ_LOG_ERR, "Error: SRV lookup failed (%d).", status);
		/* FIXME - calling on_disconnect here isn't correct. */
		pthread_mutex_lock(&mosq->callback_mutex);
		if(mosq->on_disconnect){
			mosq->in_callback = true;
			mosq->on_disconnect(mosq, mosq->userdata, 2);
			mosq->in_callback = false;
		}
		pthread_mutex_unlock(&mosq->callback_mutex);
	}
}
#endif

int mosquitto_connect_srv(struct mosquitto *mosq, const char *host, int keepalive, const char *bind_address)
{
#ifdef WITH_SRV
	char *h;
	int rc;
	if(!mosq) return MOSQ_ERR_INVAL;

	rc = ares_init(&mosq->achan);
	if(rc != ARES_SUCCESS){
		return MOSQ_ERR_UNKNOWN;
	}

	if(!host){
		// get local domain
	}else{
#ifdef WITH_TLS
		if(mosq->tls_cafile || mosq->tls_capath || mosq->tls_psk){
			h = _mosquitto_malloc(strlen(host) + strlen("_secure-mqtt._tcp.") + 1);
			if(!h) return MOSQ_ERR_NOMEM;
			sprintf(h, "_secure-mqtt._tcp.%s", host);
		}else{
#endif
			h = _mosquitto_malloc(strlen(host) + strlen("_mqtt._tcp.") + 1);
			if(!h) return MOSQ_ERR_NOMEM;
			sprintf(h, "_mqtt._tcp.%s", host);
#ifdef WITH_TLS
		}
#endif
		ares_search(mosq->achan, h, ns_c_in, ns_t_srv, srv_callback, mosq);
		_mosquitto_free(h);
	}

	pthread_mutex_lock(&mosq->state_mutex);
	mosq->state = mosq_cs_connect_srv;
	pthread_mutex_unlock(&mosq->state_mutex);

	mosq->keepalive = keepalive;

	return MOSQ_ERR_SUCCESS;

#else
	return MOSQ_ERR_NOT_SUPPORTED;
#endif
}


