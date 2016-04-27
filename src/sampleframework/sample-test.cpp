#include "sample.hpp"

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

int main(int argc, char** argv)
{
	auto helloSample = new HelloSample();
	helloSample->init();

	helloSample->run();

	helloSample->destroy();

	if (helloSample) {
		delete helloSample;
		helloSample = nullptr;
	}

	return 0;
}
