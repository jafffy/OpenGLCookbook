#include "sample.hpp"

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <vector>

#include <signal.h>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

class FBOSample : public Sample
{
public:
	virtual bool initContents();
	virtual void destroyContents();
	virtual void update(float dt);
	virtual void render();

private:
	GLuint _contentVAO, _contentVBO;
	GLuint _contentTransformBO;
	GLuint _contentTransformTBO;

	GLuint _contentProgram;
	GLuint _contentVPID;
	GLuint _contentMsID;

	float _globalTimer;

	GLuint _fboVAO, _fboVBO;

	GLuint _fboProgram;
};

Sample* sample = nullptr;

void interruptHandler(int signal)
{
	if (sample == nullptr)
		return;

	sample->destroy();

	if (sample) {
		delete sample;
		sample = nullptr;
	}

	printf("interrupted!\n");

	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
	if (signal(SIGINT, interruptHandler) == SIG_ERR) {
		fprintf(stderr, "cannot catch signal\n");
	}

	sample = new FBOSample();
	sample->init();

	sample->run();

	sample->destroy();

	if (sample) {
		delete sample;
		sample = nullptr;
	}

	return 0;
}

bool FBOSample::initContents()
{
	glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

	// Init Content
	glGenVertexArrays(1, &_contentVAO);
	assert(_contentVAO != -1);

	glBindVertexArray(_contentVAO);
	{
		_contentProgram = LoadShaders("content.vert", "content.frag");
		assert(_contentProgram != -1);

		static const GLfloat g_vertex_buffer_data[] = {
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 0.0f,  1.0f, 0.0f,
		};

		glGenBuffers(1, &_contentVBO);
		assert(_contentVBO != -1);

		glBindBuffer(GL_ARRAY_BUFFER, _contentVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		GLfloat* transform_buffer_data = new float[16 * 4];
		{
			for (int i = 0; i < 4; ++i) {
				auto T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float)i, 0.0f));
				auto R = glm::rotate(glm::mat4(1.0f), _globalTimer, glm::vec3(0.0f, 1.0f, 0.0f));
				auto S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

				auto M = T * R * S;

				memcpy(transform_buffer_data + 16 * i, glm::value_ptr(M), sizeof(float) * 16);
			}

			glGenBuffers(1, &_contentTransformBO);
			glBindBuffer(GL_TEXTURE_BUFFER, _contentTransformBO);
			glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * 16 * 4,
					transform_buffer_data, GL_STATIC_DRAW);

			glGenTextures(1, &_contentTransformTBO);
			glBindTexture(GL_TEXTURE_BUFFER, _contentTransformTBO);
			glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, _contentTransformBO);
		}
		if (transform_buffer_data) {
			delete[] transform_buffer_data;
			transform_buffer_data = nullptr;
		}
	}
	glBindVertexArray(0);

	_contentVPID = glGetUniformLocation(_contentProgram, "VP");
	_contentMsID = glGetUniformLocation(_contentProgram, "Ms");

	_globalTimer = 0.0f;

	return true;
}

void FBOSample::destroyContents()
{
	glDeleteVertexArrays(1, &_contentVAO);
	glDeleteBuffers(1, &_contentVBO);
	glDeleteBuffers(1, &_contentTransformBO);

	glDeleteTextures(1, &_contentTransformTBO);

	glDeleteProgram(_contentProgram);
}

void FBOSample::update(float dt)
{
	glBindVertexArray(_contentVAO);
	glBindBuffer(GL_TEXTURE_BUFFER, _contentTransformBO);

	float* pointer = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, sizeof(float) * 16 * 4, GL_MAP_WRITE_BIT);
	assert(pointer);

	for (int i = 0; i < 4; ++i) {
		auto T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float)i, 0.0f));
		auto R = glm::rotate(glm::mat4(1.0f), _globalTimer, glm::vec3(0.0f, 1.0f, 0.0f));
		auto S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		auto M = T * R * S;

		memcpy(pointer + 16 * i, glm::value_ptr(M), sizeof(float) * 16);
	}

	glUnmapBuffer(GL_TEXTURE_BUFFER);

	auto V = glm::lookAt(
			glm::vec3(3,4,10),
			glm::vec3(0,0,0),
			glm::vec3(0,1,0)
			);

	auto P = glm::perspective(45.0f, (float)windowWidth() / windowHeight(),
			0.1f, 100.0f);

	auto VP = P * V;

	glUseProgram(_contentProgram);
	glUniform1i(_contentMsID, 0);
	glUniformMatrix4fv(_contentVPID, 1, false, glm::value_ptr(VP));
	glUseProgram(0);

	_globalTimer += dt;
}

void FBOSample::render()
{
	glViewport(0, 0, windowWidth(), windowHeight());
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(_contentProgram);

	glBindVertexArray(_contentVAO);
	{
		glEnableVertexAttribArray(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, _contentTransformTBO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 4);

		glDisableVertexAttribArray(0);
	}

	glUseProgram(0);
}


