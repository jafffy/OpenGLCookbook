#include "sample.hpp"

#include <cstdio>
#include <cstdlib>

#include <signal.h>

class TriangleSample : public Sample
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

Sample* triangleSample = nullptr;

void funCatch(int signal)
{
	if (triangleSample == nullptr)
		return;

	triangleSample->destroy();

	if (triangleSample) {
		delete triangleSample;
		triangleSample = nullptr;
	}

	printf("interrupted!\n");

	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
	if (signal(SIGINT, funCatch) == SIG_ERR) {
		fprintf(stderr, "cannot catch signal\n");
	}

	triangleSample = new TriangleSample();
	triangleSample->init();

	triangleSample->run();

	triangleSample->destroy();

	if (triangleSample) {
		delete triangleSample;
		triangleSample = nullptr;
	}

	return 0;
}
