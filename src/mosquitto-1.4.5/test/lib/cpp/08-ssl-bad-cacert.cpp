#include <mosquittopp.h>

static int run = -1;

class mosquittopp_test : public mosqpp::mosquittopp
{
	public:
		mosquittopp_test(const char *id);
};

mosquittopp_test::mosquittopp_test(const char *id) : mosqpp::mosquittopp(id)
{
}

int main(int argc, char *argv[])
{
	struct mosquittopp_test *mosq;
	int rc = 1;

	mosqpp::lib_init();

	mosq = new mosquittopp_test("08-ssl-bad-cacert");

	mosq->tls_opts_set(1, "tlsv1", NULL);
	if(mosq->tls_set("this/file/doesnt/exist") == MOSQ_ERR_INVAL){
		rc = 0;
	}
	mosqpp::lib_cleanup();

	return rc;
}
