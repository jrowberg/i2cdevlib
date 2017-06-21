#include <cstdlib>
#include <cstring>

#include <mosquittopp.h>

static int run = -1;

class mosquittopp_test : public mosqpp::mosquittopp
{
	public:
		mosquittopp_test(const char *id);

		void on_connect(int rc);
		void on_disconnect(int rc);
		void on_publish(int mid);
};

mosquittopp_test::mosquittopp_test(const char *id) : mosqpp::mosquittopp(id)
{
}

void mosquittopp_test::on_connect(int rc)
{
	if(rc){
		exit(1);
	}else{
		publish(NULL, "pub/qos1/test", strlen("message"), "message", 1, false);
	}
}

void mosquittopp_test::on_disconnect(int rc)
{
	run = 0;
}

void mosquittopp_test::on_publish(int mid)
{
	disconnect();
}

int main(int argc, char *argv[])
{
	struct mosquittopp_test *mosq;

	mosqpp::lib_init();

	mosq = new mosquittopp_test("publish-qos1-test");
	mosq->message_retry_set(3);

	mosq->connect("localhost", 1888, 60);

	while(run == -1){
		mosq->loop();
	}

	mosqpp::lib_cleanup();

	return run;
}

