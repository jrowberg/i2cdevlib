#include <cstring>
#include <mosquittopp.h>

static int run = -1;

class mosquittopp_test : public mosqpp::mosquittopp
{
	public:
		mosquittopp_test(const char *id, bool clean_session);
};

mosquittopp_test::mosquittopp_test(const char *id, bool clean_session) : mosqpp::mosquittopp(id, clean_session)
{
}

int main(int argc, char *argv[])
{
	struct mosquittopp_test *mosq;

	mosqpp::lib_init();

	mosq = new mosquittopp_test("01-no-clean-session", false);

	mosq->connect("localhost", 1888, 60);

	while(run == -1){
		mosq->loop();
	}

	mosqpp::lib_cleanup();

	return run;
}
