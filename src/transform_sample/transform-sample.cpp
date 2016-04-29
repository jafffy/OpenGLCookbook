#include "sample.hpp"

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <signal.h>

#include <GL/glew.h>

#include "shader.hpp"

class TransformSample : public Sample
{
public:
	virtual bool initContents()
	{
		glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		
		if (_vao == -1)
			return false;

		_program = LoadShaders("transform.vert", "transform.frag");
		assert(_program != -1);

		static const GLfloat g_vertex_buffer_data[] = {
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 0.0f,  1.0f, 0.0f,
		};

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		assert(_vbo != -1);

		glBindVertexArray(0);


		return true;
	}

	virtual void destroyContents()
	{
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);

		glDeleteProgram(_program);
	}

	virtual void update(float dt)
	{
	}

	virtual void render()
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(_program);

		glBindVertexArray(_vao);

		glEnableVertexAttribArray(0);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(0);

		glUseProgram(0);
	}

private:
	GLuint _vao, _vbo;

	GLuint _program;
};

Sample* transformSample = nullptr;

void funCatch(int signal)
{
	if (transformSample == nullptr)
		return;

	transformSample->destroy();

	if (transformSample) {
		delete transformSample;
		transformSample = nullptr;
	}

	printf("interrupted!\n");

	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
	if (signal(SIGINT, funCatch) == SIG_ERR) {
		fprintf(stderr, "cannot catch signal\n");
	}

	transformSample = new TransformSample();
	transformSample->init();

	transformSample->run();

	transformSample->destroy();

	if (transformSample) {
		delete transformSample;
		transformSample = nullptr;
	}

	return 0;
}
