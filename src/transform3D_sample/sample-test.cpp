#include "sample.hpp"

#include <cstdio>
#include <cstdlib>

#include <signal.h>

class HelloSample : public Sample
{
public:
	virtual bool initContents()
	{
		return true;
	}

	virtual void destroyContents()
	{
	}

	virtual void update(float dt)
	{
	}

	virtual void render()
	{
	}
};

Sample* helloSample = nullptr;

void funCatch(int signal)
{
	if (helloSample == nullptr)
		return;

	helloSample->destroy();

	if (helloSample) {
		delete helloSample;
		helloSample = nullptr;
	}

	printf("interrupted!\n");

	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
	if (signal(SIGINT, funCatch) == SIG_ERR) {
		fprintf(stderr, "cannot catch signal\n");
	}

	helloSample = new HelloSample();
	helloSample->init();

	helloSample->run();

	helloSample->destroy();

	if (helloSample) {
		delete helloSample;
		helloSample = nullptr;
	}

	return 0;
}
