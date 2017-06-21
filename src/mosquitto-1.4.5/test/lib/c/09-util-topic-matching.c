#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>

void do_check(const char *sub, const char *topic, bool bad_res)
{
	bool match;

	mosquitto_topic_matches_sub(sub, topic, &match);
	
	if(match == bad_res){
		printf("s: %s t: %s\n", sub, topic);
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	do_check("test/6/#", "test/3", true);
	do_check("foo/bar", "foo/bar", false);
	do_check("foo/+", "foo/bar", false);
	do_check("foo/+/baz", "foo/bar/baz", false);

	do_check("A/B/+/#", "A/B/B/C", false);

	do_check("foo/+/#", "foo/bar/baz", false);
	do_check("#", "foo/bar/baz", false);

	do_check("foo/bar", "foo", true);
	do_check("foo/+", "foo/bar/baz", true);
	do_check("foo/+/baz", "foo/bar/bar", true);

	do_check("foo/+/#", "fo2/bar/baz", true);

	do_check("#", "/foo/bar", false);
	do_check("/#", "/foo/bar", false);
	do_check("/#", "foo/bar", true);


	do_check("foo//bar", "foo//bar", false);
	do_check("foo//+", "foo//bar", false);
	do_check("foo/+/+/baz", "foo///baz", false);
	do_check("foo/bar/+", "foo/bar/", false);

	do_check("$SYS/bar", "$SYS/bar", false);
	do_check("#", "$SYS/bar", true);
	do_check("$BOB/bar", "$SYS/bar", true);

	return 0;
}

