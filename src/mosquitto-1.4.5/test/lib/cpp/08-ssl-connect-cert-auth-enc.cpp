#include <cstring>
#include <mosquittopp.h>

static int run = -1;

static int password_callback(char* buf, int size, int rwflag, void* userdata)
{
	strncpy(buf, "password", size);
	buf[size-1] = '\0';

	return strlen(buf);
}

class mosquittopp_test : public mosqpp::mosquittopp
{
	public:
		mosquittopp_test(const char *id);

		void on_connect(int rc);
		void on_disconnect(int rc);
};

mosquittopp_test::mosquittopp_test(const char *id) : mosqpp::mosquittopp(id)
{
}

void mosquittopp_test::on_connect(int rc)
{
	if(rc){
		exit(1);
	}else{
		disconnect();
	}
}

void mosquittopp_test::on_disconnect(int rc)
{
	run = rc;
}


int main(int argc, char *argv[])
{
	struct mosquittopp_test *mosq;

	mosqpp::lib_init();

	mosq = new mosquittopp_test("08-ssl-connect-crt-auth-enc");

	mosq->tls_opts_set(1, "tlsv1", NULL);
	//mosq->tls_set("../ssl/test-ca.crt", NULL, "../ssl/client.crt", "../ssl/client.key");
	mosq->tls_set("../ssl/all-ca.crt", NULL, "../ssl/client-encrypted.crt", "../ssl/client-encrypted.key", password_callback);
	mosq->connect("localhost", 1888, 60);

	while(run == -1){
		mosq->loop();
	}

	mosqpp::lib_cleanup();

	return run;
}
