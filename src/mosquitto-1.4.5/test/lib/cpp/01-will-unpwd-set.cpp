#include <cstring>
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

	mosqpp::lib_init();

	mosq = new mosquittopp_test("01-will-unpwd-set");
	mosq->username_pw_set("oibvvwqw", "#'^2hg9a&nm38*us");
	mosq->will_set("will-topic", strlen("will message"), "will message", 2, false);

	mosq->connect("localhost", 1888, 60);

	while(run == -1){
		mosq->loop();
	}

	mosqpp::lib_cleanup();

	return run;
}
