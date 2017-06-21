#include <errno.h>
#include <mosquittopp.h>

static int run = -1;

class mosquittopp_test : public mosqpp::mosquittopp
{
	public:
		mosquittopp_test(const char *id);

		void on_connect(int rc);
};

mosquittopp_test::mosquittopp_test(const char *id) : mosqpp::mosquittopp(id)
{
}

void mosquittopp_test::on_connect(int rc)
{
	exit(1);
}

int main(int argc, char *argv[])
{
	struct mosquittopp_test *mosq;
	int rc;

	mosqpp::lib_init();

	mosq = new mosquittopp_test("08-ssl-fake-cacert");

	mosq->tls_opts_set(1, "tlsv1", NULL);
	mosq->tls_set("../ssl/test-fake-root-ca.crt", NULL, "../ssl/client.crt", "../ssl/client.key");
	mosq->connect("localhost", 1888, 60);

	rc = mosq->loop_forever();
	if(rc == MOSQ_ERR_ERRNO && errno == EPROTO){
		return 0;
	}else{
		return 1;
	}
}
