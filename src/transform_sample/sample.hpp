#ifndef SAMPLE_H_
#define SAMPLE_H_

class Sample_Impl;

class Sample
{
public:
	Sample();
	virtual ~Sample();

	virtual bool init();
	virtual void destroy();

	virtual bool initContents() = 0;
	virtual void destroyContents() = 0;

	virtual void update(float dt) = 0;
	virtual void render() = 0;

	virtual bool is_running() const;

	virtual void run();

	virtual int windowWidth() const;
	virtual int windowHeight() const;

protected:
	Sample_Impl* impl;
};

#endif // SAMPLE_H_
